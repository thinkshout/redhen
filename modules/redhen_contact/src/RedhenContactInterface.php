<?php

/**
 * @file
 * Contains \Drupal\redhen_contact\RedhenContactInterface.
 */

namespace Drupal\redhen_contact;

use Drupal\Core\Entity\ContentEntityInterface;
use Drupal\Core\Entity\EntityChangedInterface;
use Drupal\Core\Entity\EntityTypeInterface;
use Drupal\user\EntityOwnerInterface;

/**
 * Provides an interface for defining Contact entities.
 *
 * @ingroup redhen_contact
 */
interface RedhenContactInterface extends ContentEntityInterface, EntityChangedInterface, EntityOwnerInterface {
  // Add get/set methods for your configuration properties here.

  /**
   * Gets the Contact type.
   *
   * @return string
   *   The Contact type.
   */
  public function getType();

  /**
   * Gets the Contact name.
   *
   * @return string
   *   Name of the Contact.
   */
  public function getName();

  /**
   * Sets the Contact name.
   *
   * @param string $name
   *   The Contact name.
   *
   * @return \Drupal\redhen_contact\RedhenContactInterface
   *   The called Contact entity.
   */
  public function setName($name);

  /**
   * Gets the Contact creation timestamp.
   *
   * @return int
   *   Creation timestamp of the Contact.
   */
  public function getCreatedTime();

  /**
   * Sets the Contact creation timestamp.
   *
   * @param int $timestamp
   *   The Contact creation timestamp.
   *
   * @return \Drupal\redhen_contact\RedhenContactInterface
   *   The called Contact entity.
   */
  public function setCreatedTime($timestamp);

  /**
   * Returns the Contact published status indicator.
   *
   * Unpublished Contact are only visible to restricted users.
   *
   * @return bool
   *   TRUE if the Contact is published.
   */
  public function isPublished();

  /**
   * Sets the published status of a Contact.
   *
   * @param bool $published
   *   TRUE to set this Contact to published, FALSE to set it to unpublished.
   *
   * @return \Drupal\redhen_contact\RedhenContactInterface
   *   The called Contact entity.
   */
  public function setPublished($published);

}
