<?php

namespace Drupal\redhen_connection;

use Drupal\Core\Entity\ContentEntityInterface;
use Drupal\Core\Entity\EntityChangedInterface;

/**
 * Provides an interface for defining Connection entities.
 *
 * @ingroup redhen_connection
 */
interface ConnectionInterface extends ContentEntityInterface, EntityChangedInterface {

  /**
   * Gets the Connection type.
   *
   * @return string
   *   The Connection type.
   */
  public function getType();

  /**
   * Gets the Connection creation timestamp.
   *
   * @return int
   *   Creation timestamp of the Connection.
   */
  public function getCreatedTime();

  /**
   * Sets the Connection creation timestamp.
   *
   * @param int $timestamp
   *   The Connection creation timestamp.
   *
   * @return \Drupal\redhen_connection\ConnectionInterface
   *   The called Connection entity.
   */
  public function setCreatedTime($timestamp);

  /**
   * Returns a label for the Connection.
   */
  public function label();

  /**
   * Returns the Connection active status indicator.
   *
   * @return bool
   *   TRUE if the Connection is active.
   */
  public function isActive();

  /**
   * Sets the active status of a Connection.
   *
   * @param bool $active
   *   TRUE to set this Connection to active, FALSE to set it to inactive.
   *
   * @return \Drupal\redhen_connection\ConnectionInterface
   *   The called Connection entity.
   */
  public function setActive($active);

}
