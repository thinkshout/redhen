<?php

namespace Drupal\redhen_connection;

use Doctrine\Entity;
use Drupal\Core\Access\AccessResultInterface;
use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Session\AccountInterface;

/**
 * Provides an interface for getting connections between entities.
 */
interface ConnectionServiceInterface {


  /**
   * Returns the connection types that can be connected to this entity.
   *
   * @param \Drupal\Core\Entity\EntityInterface $entity
   *
   * @return array
   */
  public function getConnectionTypes(EntityInterface $entity);

  /**
   * Returns the connections to this entity.
   *
   * @param EntityInterface $entity
   *   The entity we're querying against.
   * @param string $connection_type
   *   (optional) Limit returned connections to this type.
   *
   * @return array
   *   The Connection entities connected to this entity.
   */
  public function getConnections(EntityInterface $entity, $connection_type = NULL);

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