<?php

namespace Drupal\redhen_connection;

use Drupal\Core\Access\AccessResultInterface;
use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Session\AccountInterface;

/**
 * Provides an interface for getting connections between entities.
 */
interface ConnectionServiceInterface {


  /**
   * Returns the connection types that can be connected to a single entity or two
   * entities.
   *
   * @param \Drupal\Core\Entity\EntityInterface $entity
   * @param \Drupal\Core\Entity\EntityInterface $entity2
   *
   * @return array
   */
  public function getConnectionTypes(EntityInterface $entity, EntityInterface $entity2 = NULL);

  /**
   * Returns the connections to this entity.
   *
   * @param EntityInterface $entity
   *   The entity we're querying against.
   * @param EntityInterface $entity2
   *   The second entity we're querying against.
   * @param string $connection_type
   *   (optional) Limit returned connections to this type.
   * @param bool $active
   *   (optional) Return only active connections.
   * @param array $sort
   *   (optional) Associative array of field to sort by and direction:
   *   e.g. ['field_name'] => 'DESC'
   * @param int $offset
   *   The number of records to offset results by.
   * @param int $limit
   *   The number of records to limit results to.
   *
   * @return array
   *   The Connection entities connected to this entity.
   */
  public function getConnections(EntityInterface $entity, EntityInterface $entity2 = NULL, $connection_type = NULL, $active = TRUE, $sort = array(), $offset = 0, $limit = 0);

  /**
   * Returns the number of connections to this entity.
   *
   * @param EntityInterface $entity
   *   The entity we're querying against.
   * @param EntityInterface $entity2
   *   The entity we're querying against.
   * @param string $connection_type
   *   (optional) Limit returned connections to this type.
   *
   * @return int
   *   The number of Connection entities connected to this entity.
   */
  public function getConnectionCount(EntityInterface $entity, EntityInterface $entity2 = NULL, $connection_type = NULL);

  /**
   * Returns the indirect connections to this entity.
   *
   * @param EntityInterface $entity
   *   The entity we're querying against.
   * @param EntityInterface $entity2
   *   The second entity we're querying against.
   * @param string $connection_type
   *   (optional) Limit returned connections to this type.
   * @param bool $active
   *   (optional) Limit to active connections.
   *
   * @return array
   *   The Connection entities connected to this entity.
   */
  public function getIndirectConnections(EntityInterface $entity, EntityInterface $entity2, $connection_type = NULL, $active = TRUE);

  /**
   * Returns the other entities that are connected to this entity.
   *
   * @param EntityInterface $entity
   *   The entity we're querying against.
   * @param string $connection_type
   *   (optional) Limit returned entities to ones connected via this type.
   *
   * @return array
   *   The connected entities for this entity.
   */
  public function getConnectedEntities(EntityInterface $entity, $connection_type = NULL);

  /**
   * Check access to an entity via its connections and the current users connection roles.
   *
   * @param \Drupal\Core\Entity\EntityInterface $entity
   *   The entity to check against.
   * @param $operation
   *   The entity operation (view, view label, update, delete, create)
   * @param \Drupal\Core\Session\AccountInterface|NULL $account
   *   The User to check against.
   *
   * @return AccessResultInterface
   */
  public function checkConnectionPermission(EntityInterface $entity, $operation, AccountInterface $account = NULL);

}