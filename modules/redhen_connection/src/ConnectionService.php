<?php

namespace Drupal\redhen_connection;

use Drupal\Core\Access\AccessResultAllowed;
use Drupal\Core\Database\Connection as DBConnection;
use Drupal\Core\Entity\Entity;
use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Entity\EntityTypeManagerInterface;
use Drupal\Core\Entity\Query\QueryFactory;
use Drupal\Core\Entity\Query\QueryInterface;
use Drupal\Core\Session\AccountInterface;
use Drupal\redhen_connection\Entity\ConnectionType;
use Drupal\redhen_connection\Entity\Connection;
use Drupal\redhen_contact\ContactInterface;
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
   * The database connection to use.
   *
   * @var \Drupal\Core\Database\Connection
   */
  protected $connection;

  /**
   * Constructs a EntityCreateAccessCheck object.
   *
   * @param \Drupal\Core\Entity\EntityTypeManagerInterface $entity_type_manager
   *   The entity manager.
   * @param \Drupal\Core\Entity\Query\QueryFactory $entity_query
   *   The entity query.
   * @param \Drupal\Core\Database\Connection $connection
   *   The database connection.
   */
  public function __construct(EntityTypeManagerInterface $entity_type_manager, QueryFactory $entity_query, DBConnection $connection) {
    $this->entityTypeManager = $entity_type_manager;
    $this->entityQuery = $entity_query;
    $this->connection = $connection;
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
      $connection_types = ConnectionType::loadMultiple($results);
    }

    return $connection_types;
  }

  /**
   * {@inheritdoc}
   */
  public function getConnections(EntityInterface $entity, EntityInterface $entity2 = NULL, $connection_type = NULL, $active = TRUE, $sort = array(), $offset = 0, $limit = 0) {
    $connections = [];

    $query = $this->buildQuery($entity, $entity2, $connection_type, $active);

    if ($query) {

      foreach ($sort as $field => $direction) {
        $query->sort($field, $direction);
      }

      if ($limit > 0) {
        $query->range($offset, $limit);
      }

      $results = $query->execute();

      if (!empty($results)) {
        $connections = Connection::loadMultiple($results);
      }
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
    $connected_entities = array();

    $type = ConnectionType::load($connection_type);

    // Get all fields that reference endpoints on the connection type.
    $fields = $type->getAllEndpointFields();

    // Get connections.
    $connections = $this->getConnections($entity, NULL, $connection_type);

    // Loop through connections to find entities referenced by endpoint fields.
    foreach ($connections as $connection) {
      foreach ($fields as $field) {
        $referenced_entities = $connection->get($field)->referencedEntities();

        foreach ($referenced_entities as $referenced_entity) {
          // Do not include the entity originally passed in function args.
          if (($referenced_entity->getEntityType()->id() == $entity->getEntityType()->id()) && ($referenced_entity->id() == $entity->id())) {
            continue;
          }

          $connected_entities[] = $referenced_entity;
        }
      }
    }

    return $connected_entities;
  }

  /**
   * {@inheritdoc}
   */
  public function getIndirectConnections(EntityInterface $entity, EntityInterface $entity2, $connection_type = NULL, $active = TRUE) {
    $types = ($connection_type) ? [$connection_type => ConnectionType::load($connection_type)] : $this->getConnectionTypes($entity);

    $results = [];
    foreach ($types as $type_id => $connection_type) {
      /** @var ConnectionType $connection_type */
      // Get endpoints (usually 1, but two possible).
      $endpoints = $connection_type->getEndpointFields($entity->getEntityTypeId());
      // Use first endpoint only.
      $endpoint = current($endpoints);

      // @todo Don't hardcode the endpoint names.
      $shared_endpoint = ($endpoint == 'endpoint_1') ? 'endpoint_2' : 'endpoint_1';

      // Build our subquery first.
      $join_query = $this->connection->select('redhen_connection', 'sub');
      $join_query->addField('sub', 'id');
      $join_query->addField('sub', 'type');
      $join_query->addField('sub', $endpoint);
      $join_query->addField('sub', $shared_endpoint);
      $join_query->condition('sub.' . $endpoint, $entity2->id());

      // Base table is filtered on the first entity's id.
      $query = $this->connection->select('redhen_connection', 'c');
      $query->addField('c', 'id');
      $query->condition('c. '. $endpoint, $entity->id());

      // If we're filtering on active connections (default) limit status.
      if ($active) {
        $join_query->condition('sub.status', 1);
        $query->condition('c.status', 1);
      }

      // Join on type and endpoint match. Can't pass $endpoint as argument
      // because it will be automatically wrapped in quotes and break the SQL.
      $query->innerJoin($join_query, 'c2', 'c.type = c2.type AND c.' . $shared_endpoint . ' = c2.' . $shared_endpoint);

      $result = $query->execute();

      $type_results = $result->fetchCol(0);
      $results = array_merge($results, $type_results);
    }

    $connections = [];
    if (!empty($results)) {
      $connections = Connection::loadMultiple($results);
    }

    return $connections;
  }

  /**
   * {@inheritdoc}
   */
  public function checkConnectionPermission(EntityInterface $entity, $operation, AccountInterface $account = NULL) {
    // Get connections and loop through checking for role permissions.
    $contact = Contact::loadByUser($account);
    if ($contact) {
      $direct_connections = $this->getConnections($contact, $entity);
      foreach ($direct_connections as $connection) {
        /** @var ConnectionInterface $connection */
        if ($result = $connection->hasRolePermission($entity, $operation, $contact)) {
          return new AccessResultAllowed();
        }
      }
      // Separate from the direct connections check because we want to limit
      // checking for indirect connections to only when no direct connection
      // returned AccessResultAllowed. We also only want to check if the entity
      // we're checking on is a Contact.
      if ($entity->getEntityTypeId() == 'redhen_contact') {
        $indirect_connections = $this->getIndirectConnections($contact, $entity);
        foreach ($indirect_connections as $connection) {
          /** @var ConnectionInterface $connection */
          if ($result = $connection->hasRolePermission($entity, $operation, $contact)) {
            return new AccessResultAllowed();
          }
        }
      }
    }
    // @todo - we should be able to return a neutral result here - test again to see if we can
  }

  /**
   * @param \Drupal\Core\Entity\EntityInterface $entity
   *   The entity we're querying against.
   * @param \Drupal\Core\Entity\EntityInterface $entity2
   *   The second entity we're querying against.
   * @param null $connection_type
   *   Limit query to this connection type.
   * @param bool $active
   *   Only active connections.
   *
   * @return QueryInterface
   */
  private function buildQuery(EntityInterface $entity, EntityInterface $entity2 = NULL, $connection_type = NULL, $active = TRUE) {
    $types = ($connection_type) ? [$connection_type => ConnectionType::load($connection_type)] : $this->getConnectionTypes($entity, $entity2);

    /** @var QueryInterface $query */
    $query = $this->entityQuery->get('redhen_connection');

    // Add condition for the connection status.
    if ($active) {
      $query->condition('status', 1);
    }

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

    // @todo Might instead be able to query against endpoint_1.entity.type, etc.

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
            for ($x=count($endpoint_fields) - 1; $x >= 0; $x--) {
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
