<?php

namespace Drupal\redhen_connection;

use Drupal\Core\Entity\EntityInterface;

/**
 * Provides an interface for getting connections between entities.
 */
interface ConnectionServiceInterface {

  /**
   * Filters entity list to bundle definitions for entities w/ connection types.
   *
   * @param EntityTypeInterface[] $entity_types
   *   The master entity type list filter.
   *
   * @return ConfigEntityTypeInterface[]
   *   An array of only the config entities we want to modify.
   */
  public function getConnectionEntityTypes(array $entity_types);

  /**
   * Returns connection types that can be connected to 1 or 2 entities.
   *
   * @param \Drupal\Core\Entity\EntityInterface $entity
   *   Entity 1.
   * @param \Drupal\Core\Entity\EntityInterface $entity2
   *   Entity 2.
   *
   * @return array
   *   Connection types that can be used between the 1 or 2 entities.
   */
  public function getConnectionTypes(EntityInterface $entity, EntityInterface $entity2 = NULL);

  /**
   * Returns the connections to this entity.
   *
   * @param \Drupal\Core\Entity\EntityInterface $entity
   *   The entity we're querying against.
   * @param \Drupal\Core\Entity\EntityInterface $entity2
   *   The second entity we're querying against.
   * @param string $connection_type
   *   (optional) Limit returned connections to this type.
   * @param bool $active
   *   (optional) Return only active connections.
   *
   * @return array
   *   The Connection entities connected to this entity.
   */
  public function getConnections(EntityInterface $entity, EntityInterface $entity2 = NULL, $connection_type = NULL, $active = TRUE);

  /**
   * Returns the number of connections to this entity.
   *
   * @param \Drupal\Core\Entity\EntityInterface $entity
   *   The entity we're querying against.
   * @param \Drupal\Core\Entity\EntityInterface $entity2
   *   The entity we're querying against.
   * @param string $connection_type
   *   (optional) Limit returned connections to this type.
   *
   * @return int
   *   The number of Connection entities connected to this entity.
   */
  public function getConnectionCount(EntityInterface $entity, EntityInterface $entity2 = NULL, $connection_type = NULL);

  /**
   * Returns the other entities that are connected to this entity.
   *
   * @param \Drupal\Core\Entity\EntityInterface $entity
   *   The entity we're querying against.
   * @param string $connection_type
   *   (optional) Limit returned entities to ones connected via this type.
   *
   * @return array
   *   The connected entities for this entity.
   */
  public function getConnectedEntities(EntityInterface $entity, $connection_type = NULL);

  /**
   * Check entity access via its connections & current user's connection roles.
   *
   * @param \Drupal\Core\Entity\EntityInterface $endpoint1
   *   Endpoint 1 of the connection.
   * @param \Drupal\Core\Entity\EntityInterface $endpoint2
   *   Endpoint 2 of the connection.
   * @param string $operation
   *   The entity operation (view, view label, update, delete, create)
   * @param string $permission_key
   *   Key for checking permissions against.
   *
   * @return \Drupal\Core\Access\AccessResultInterface
   *   Access result, either neutral or allowed.
   */
  public function checkConnectionPermission(EntityInterface $endpoint1, EntityInterface $endpoint2, $operation, $permission_key);

  /**
   * Get all entities that are used in connections.
   *
   * @return array
   *   An array of entity_types.
   */
  public function getAllConnectionEntityTypes();

}
