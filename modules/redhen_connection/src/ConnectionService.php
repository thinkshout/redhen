<?php

namespace Drupal\redhen_connection;

use Drupal\Core\Access\AccessResultNeutral;
use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Entity\EntityTypeManagerInterface;
use Drupal\Core\Entity\Query\QueryFactory;
use Drupal\Core\Entity\Query\QueryInterface;
use Drupal\Core\Session\AccountInterface;
use Drupal\redhen_connection\Entity\ConnectionType;
use Drupal\redhen_connection\Entity\Connection;

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
  public function getConnectionTypes(EntityInterface $entity) {
    $entity_type = $entity->getEntityTypeId();
    $query = $this->entityQuery->get('redhen_connection_type', 'OR');
    // @todo add conditions based on REDHEN_CONNECTION_ENDPOINTS constant.
    $query->condition('endpoints.1.entity_type', $entity_type);
    $query->condition('endpoints.2.entity_type', $entity_type);
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
  public function getConnections(EntityInterface $entity, $connection_type = NULL, $sort = array(), $offset = 0, $limit = 0) {

    $query = $this->buildQuery($entity, $connection_type);

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
  public function getConnectionCount(EntityInterface $entity, $connection_type = NULL) {
    $query = $this->buildQuery($entity, $connection_type);

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

    return new AccessResultNeutral();
  }

  /**
   * @param \Drupal\Core\Entity\EntityInterface $entity
   *   The entity we're querying against.
   * @param null $connection_type
   *
   * @return QueryInterface
   */
  private function buildQuery(EntityInterface $entity, $connection_type = NULL) {
    $types = ($connection_type) ? [$connection_type => ConnectionType::load($connection_type)] : $this->getConnectionTypes($entity);
    $entity_type = $entity->getEntityType()->id();

    /** @var QueryInterface $query */
    $query = $this->entityQuery->get('redhen_connection');

    // Add condition for the connection status.
    // @todo Make configurable.
    $query->condition('status', 1);

    if ($connection_type != NULL) {
      $query->condition('type', $connection_type);
    }

    // Endpoint conditions.

    $conditions = [];
    // Build endpoint query.
    foreach ($types as $type => $connection_type) {
      /** @var ConnectionTypeInterface $connection_type */
      $fields = $connection_type->getEndpointFields($entity_type);
      foreach ($fields as $field) {
        $conditions[$field][] = $type;
      }
    }

    // @todo Only want an OR group if there are more than one conditions.
    $condition_group = $query->orConditionGroup();

    foreach ($conditions as $endpoint => $condition_types) {
      $and_group = $query->andConditionGroup()->condition($endpoint, $entity->id(), '=')
        ->condition('type', $condition_types, 'IN');
      $condition_group->condition($and_group);
    }

    $query->condition($condition_group);

    return $query;
  }

}