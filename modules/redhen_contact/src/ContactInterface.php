<?php

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
interface ContactInterface extends ContentEntityInterface, EntityOwnerInterface, EntityChangedInterface {
  /**
   * Gets the Contact type.
   *
   * @return string
   *   The Contact type.
   */
  public function getType();

  /**
   * Gets the full Contact name.
   *
   * @return string
   *   Name of the Contact.
   */
  public function getFullName();

  /**
   * Sets the Contact name.
   *
   * @param string $name
   *   The Contact name.
   *
   * @return \Drupal\redhen_contact\ContactInterface
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
   * @return \Drupal\redhen_contact\ContactInterface
   *   The called Contact entity.
   */
  public function setCreatedTime($timestamp);

  /**
   * Returns a label for the contact.
   */
  public function label();

  /**
   * Returns the Contact active status indicator.
   *
   * @return bool
   *   TRUE if the Contact is active.
   */
  public function isActive();

  /**
   * Sets the active status of a Contact.
   *
   * @param bool $active
   *   TRUE to set this Contact to active, FALSE to set it to inactive.
   *
   * @return \Drupal\redhen_contact\ContactInterface
   *   The called Contact entity.
   */
  public function setActive($active);

}
