<?php

/**
 * @file
 * Contains \Drupal\redhen_relation\RelationTypeInterface.
 */

namespace Drupal\redhen_relation;

use Drupal\Core\Config\Entity\ConfigEntityInterface;

/**
 * Provides an interface for defining Relation type entities.
 */
interface RelationTypeInterface extends ConfigEntityInterface {
  /**
   * Gets the relation type's entity type ID by field.
   *
   * E.g, if relations of this type are configured to allow Contacts to
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
   * Sets the relation type's entity type ID by field.
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
