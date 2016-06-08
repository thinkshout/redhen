<?php

namespace Drupal\redhen_connection;

use Drupal\Core\Access\AccessResultAllowed;
use Drupal\Core\Access\AccessResultNeutral;
use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Entity\EntityTypeManagerInterface;
use Drupal\Core\Entity\Query\QueryFactory;
use Drupal\Core\Entity\Query\QueryInterface;
use Drupal\Core\Session\AccountInterface;
use Drupal\redhen_connection\Entity\ConnectionType;
use Drupal\redhen_connection\Entity\Connection;
use Drupal\redhen_contact\Entity\Contact;

/**
 * Provides an interface for getting connections between entities.
 */
class ConnectionService implements ConnectionServiceInterface {


  /**
   * The entity manager.
   *
   * @var \Drupal\Core\Entity\EntityTypeManagerInterface
   */
  protected $entityTypeManager;

  /**
   * The entity query.
   *
   * @var \Drupal\Core\Entity\Query\QueryFactory
   */
  protected $entityQuery;


  /**
   * Constructs a EntityCreateAccessCheck object.
   *
   * @param \Drupal\Core\Entity\EntityTypeManagerInterface $entity_type_manager
   *   The entity manager.
   * @param \Drupal\Core\Entity\Query\QueryFactory $entity_query
   *   The entity query.
   */
  public function __construct(EntityTypeManagerInterface $entity_type_manager, QueryFactory $entity_query) {
    $this->entityTypeManager = $entity_type_manager;
    $this->entityQuery = $entity_query;
  }

  /**
   * {@inheritdoc}
   */
  public function getConnectionTypes(EntityInterface $entity, EntityInterface $entity2 = NULL) {
    $query = $this->entityQuery->get('redhen_connection_type');
    $or_group = $query->orConditionGroup();
    $entity_type = $entity->getEntityTypeId();

    if (empty($entity2)) {
      // Single entity provided
      $or_group->condition('endpoints.1.entity_type', $entity_type);
      $or_group->condition('endpoints.2.entity_type', $entity_type);
    }
    else {
      // Two entities provided.
      $entity_type2 = $entity2->getEntityTypeId();
      $and_group = $query->andConditionGroup()
        ->condition('endpoints.1.entity_type', $entity_type)
        ->condition('endpoints.2.entity_type', $entity_type2);

      $and_group2 = $query->andConditionGroup()
        ->condition('endpoints.2.entity_type', $entity_type)
        ->condition('endpoints.1.entity_type', $entity_type2);

      $or_group->condition($and_group)
        ->condition($and_group2);
    }

    $query->condition($or_group);
    $results = $query->execute();

    $connection_types = [];
    if (!empty($results)) {
      $connection_types = ConnectionType::loadMultiple(array_keys($results));
    }

    return $connection_types;
  }

  /**
   * {@inheritdoc}
   */
  public function getConnections(EntityInterface $entity, EntityInterface $entity2 = NULL, $connection_type = NULL, $sort = array(), $offset = 0, $limit = 0) {

    $query = $this->buildQuery($entity, $entity2, $connection_type);

    foreach ($sort as $field => $direction) {
      $query->sort($field, $direction);
    }

    if ($limit > 0) {
      $query->range($offset, $limit);
    }

    $results = $query->execute();

    $connections = array();
    if (!empty($results))
    {
      $connections = Connection::loadMultiple(array_keys($results));
    }

    return $connections;
  }

  /**
   * {@inheritdoc}
   */
  public function getConnectionCount(EntityInterface $entity, EntityInterface $entity2 = NULL, $connection_type = NULL) {
    $query = $this->buildQuery($entity, $entity2, $connection_type);

    return $query->count()->execute();
  }

  /**
   * {@inheritdoc}
   */
  public function getConnectedEntities(EntityInterface $entity, $connection_type = NULL) {

  }

  /**
   * {@inheritdoc}
   */
  public function checkConnectionPermission(EntityInterface $entity, $operation, AccountInterface $account = NULL) {
    // Get connections and loop through checking for role permissions.
    $contact = Contact::loadByUser($account);
    if ($contact) {
      foreach ($this->getConnections($contact, $entity) as $connection) {
        /** @var ConnectionInterface $connection */
        if ($result = $connection->hasRolePermission($entity, $operation, $contact)) {
          return new AccessResultAllowed();
        }
      }
      // $this->getIndirectConnections($contact, $entity)
    }

    return new AccessResultNeutral();
  }

  /**
   * @param \Drupal\Core\Entity\EntityInterface $entity
   *   The entity we're querying against.
   * * @param \Drupal\Core\Entity\EntityInterface $entity2
   *   The second entity we're querying against.
   * @param null $connection_type
   *
   * @return QueryInterface
   */
  private function buildQuery(EntityInterface $entity, EntityInterface $entity2 = NULL, $connection_type = NULL) {
    $types = ($connection_type) ? [$connection_type => ConnectionType::load($connection_type)] : $this->getConnectionTypes($entity, $entity2);

    /** @var QueryInterface $query */
    $query = $this->entityQuery->get('redhen_connection');

    // Add condition for the connection status.
    // @todo Make configurable.
    $query->condition('status', 1);

    if ($connection_type != NULL) {
      $query->condition('type', $connection_type);
    }

    // Endpoint conditions.
    $entities = [];
    $entity_type = $entity->getEntityType()->id();
    $entities[$entity_type][] = $entity;
    $entity_type2 = ($entity2) ? $entity2->getEntityType()->id() : NULL;
    if ($entity2) {
      $entities[$entity_type2][] = $entity2;
    }

    // Overall OR group of connection_type/endpoint groupings.
    $endpoints_group = $query->orConditionGroup();

    // Build endpoint groups.
    foreach ($types as $type => $connection_type) {
      /** @var ConnectionTypeInterface $connection_type */
      $endpoints = [];
      $endpoints[$entity_type] = $connection_type->getEndpointFields($entity_type);

      // Add condition for the connection_type.
      $condition_group = $query->andConditionGroup()
        ->condition('type', $type);

      // Working with 2 endpoints.
      if ($entity_type2) {
        $group = $query->andConditionGroup();
        $endpoints[$entity_type2] = $connection_type->getEndpointFields($entity_type2);

        foreach ($endpoints as $endpoint_type => $endpoint_fields) {
          for ($x=0; $x < count($endpoint_fields); $x++) {
            $group->condition($endpoint_fields[$x], $entities[$endpoint_type][$x]->id());
          }
          // Endpoints are of the same type so we need to add an additional
          // condition for the reverse structure.
          if ($x > 1) {
            $group2 = $query->orConditionGroup();
            for ($x=count($endpoint_fields); $x >= 0; $x--) {
              $group2->condition($endpoint_fields[$x], $entities[$endpoint_type][$x]->id());
            }
            $group = $query->orConditionGroup()
              ->condition($group)
              ->condition($group2);
          }
        }

        $condition_group->condition($group);
      }
      else {
        // Single entity.
        $condition_group = $query->orConditionGroup();
        foreach ($endpoints as $endpoint_type => $endpoint_fields) {
          for ($x = 0; $x < count($endpoint_fields); $x++) {
            $condition_group->condition($endpoint_fields[$x], $entity->id());
          }
        }
      }
      $endpoints_group->condition($condition_group);
    }
    if (!empty($types)) {
      $query->condition($endpoints_group);
      return $query;
    }

    return FALSE;
  }

}