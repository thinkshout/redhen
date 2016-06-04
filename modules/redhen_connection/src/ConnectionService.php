<?php

namespace Drupal\redhen_connection;

use Drupal\Core\Access\AccessResultInterface;
use Drupal\Core\Access\AccessResultNeutral;
use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Entity\EntityTypeManagerInterface;
use Drupal\Core\Entity\Query\QueryFactory;
use Drupal\Core\Entity\Query\QueryInterface;
use Drupal\Core\Session\AccountInterface;
use Drupal\redhen_connection\ConnectionTypeInterface;
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
    $query->condition('endpoints.1.entity_type', $entity_type);
    $query->condition('endpoints.2.entity_type', $entity_type);
    $results = $query->execute();

    return (!empty($results)) ? array_keys($results) : array();
  }

  /**
   * {@inheritdoc}
   */
  public function getConnections(EntityInterface $entity, $connection_type = NULL, $sort = array()) {
    /** @var QueryInterface $query */
    $query = \Drupal::entityQuery('redhen_connection');

    $endpoints = $query->orConditionGroup()
      ->condition('endpoint_1', $entity->id())
      ->condition('endpoint_2', $entity->id());

    $query
      ->condition('status', 1)
      ->condition($endpoints);

    if ($connection_type != NULL) {
      $query->condition('type', $connection_type);
    }

    foreach ($sort as $field => $direction) {
      $query->sort($field, $direction);
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
    /** @var QueryInterface $query */
    $query = \Drupal::entityQuery('redhen_connection');

    $endpoints = $query->orConditionGroup()
      ->condition('endpoint_1', $entity->id())
      ->condition('endpoint_2', $entity->id());

    $query
      ->condition('status', 1)
      ->condition($endpoints);

    if ($connection_type != NULL) {
      $query->condition('type', $connection_type);
    }

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

}