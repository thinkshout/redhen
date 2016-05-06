<?php

/**
 * @file
 * Contains \Drupal\redhen_relation\RelationInterface.
 */

namespace Drupal\redhen_relation;

use Drupal\Core\Entity\ContentEntityInterface;
use Drupal\Core\Entity\EntityChangedInterface;

/**
 * Provides an interface for defining Relation entities.
 *
 * @ingroup redhen_relation
 */
interface RelationInterface extends ContentEntityInterface, EntityChangedInterface {
  /**
   * Gets the Relation type.
   *
   * @return string
   *   The Relation type.
   */
  public function getType();

  /**
   * Gets the Relation creation timestamp.
   *
   * @return int
   *   Creation timestamp of the Relation.
   */
  public function getCreatedTime();

  /**
   * Sets the Relation creation timestamp.
   *
   * @param int $timestamp
   *   The Relation creation timestamp.
   *
   * @return \Drupal\redhen_relation\RelationInterface
   *   The called Relation entity.
   */
  public function setCreatedTime($timestamp);

  /**
   * Returns a label for the Relation.
   */
  public function label();

  /**
   * Returns the Relation active status indicator.
   *
   * @return bool
   *   TRUE if the Relation is active.
   */
  public function isActive();

  /**
   * Sets the active status of a Relation.
   *
   * @param bool $active
   *   TRUE to set this Relation to active, FALSE to set it to inactive.
   *
   * @return \Drupal\redhen_relation\RelationInterface
   *   The called Relation entity.
   */
  public function setActive($active);

}
