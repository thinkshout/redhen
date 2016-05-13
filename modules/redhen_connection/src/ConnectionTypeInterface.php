<?php

/**
 * @file
 * Contains \Drupal\redhen_connection\ConnectionTypeInterface.
 */

namespace Drupal\redhen_connection;

use Drupal\Core\Config\Entity\ConfigEntityInterface;

/**
 * Provides an interface for defining Connection type entities.
 */
interface ConnectionTypeInterface extends ConfigEntityInterface {
  /**
   * Gets the connection type's entity type ID by field.
   *
   * E.g, if connections of this type are configured to allow Contacts to
   * connect with other Contacts, this will return redhen_contact for both
   * fields.
   *
   * @param string $field
   *   The field to return the entity type for.
   *
   * @return string
   *   The entity type ID for $field.
   */
  public function getEndpointEntityTypeId($field);
  /**
   * Sets the connection type's entity type ID by field.
   *
   * @param string $field
   *   The field to set the entity type for.

   * @param string $entity_type_id
   *   The purchasable entity type.
   *
   * @return $this
   */
  public function setEndpointEntityTypeId($field, $entity_type_id);

}
