<?php

namespace Drupal\redhen_org;

use Drupal\Core\Entity\ContentEntityInterface;
use Drupal\Core\Entity\EntityChangedInterface;

/**
 * Provides an interface for defining Org entities.
 *
 * @ingroup redhen_org
 */
interface OrgInterface extends ContentEntityInterface, EntityChangedInterface {
  /**
   * Gets the Org type.
   *
   * @return string
   *   The Org type.
   */
  public function getType();

  /**
   * Gets the Org creation timestamp.
   *
   * @return int
   *   Creation timestamp of the Org.
   */
  public function getCreatedTime();

  /**
   * Sets the Org creation timestamp.
   *
   * @param int $timestamp
   *   The Org creation timestamp.
   *
   * @return \Drupal\redhen_org\OrgInterface
   *   The called Org entity.
   */
  public function setCreatedTime($timestamp);

  /**
   * Returns a label for the org.
   */
  public function label();

  /**
   * Returns the Org active status indicator.
   *
   * @return bool
   *   TRUE if the Org is active.
   */
  public function isActive();

  /**
   * Sets the active status of a Org.
   *
   * @param bool $active
   *   TRUE to set this Org to active, FALSE to set it to inactive.
   *
   * @return \Drupal\redhen_org\OrgInterface
   *   The called Org entity.
   */
  public function setActive($active);

}
