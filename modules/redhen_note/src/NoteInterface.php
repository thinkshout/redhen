<?php

namespace Drupal\redhen_note;

use Drupal\Core\Entity\ContentEntityInterface;
use Drupal\Core\Entity\EntityChangedInterface;
use Drupal\user\EntityOwnerInterface;

/**
 * Provides an interface for defining RedHen Note entities.
 *
 * @ingroup redhen_note
 */
interface NoteInterface extends ContentEntityInterface, EntityChangedInterface, EntityOwnerInterface {
  // Add get/set methods for your configuration properties here.
  /**
   * Gets the RedHen Note name.
   *
   * @return string
   *   Name of the RedHen Note.
   */
  public function getName();

  /**
   * Sets the RedHen Note name.
   *
   * @param string $name
   *   The RedHen Note name.
   *
   * @return \Drupal\redhen_note\NoteInterface
   *   The called RedHen Note entity.
   */
  public function setName($name);

  /**
   * Gets the RedHen Note creation timestamp.
   *
   * @return int
   *   Creation timestamp of the RedHen Note.
   */
  public function getCreatedTime();

  /**
   * Sets the RedHen Note creation timestamp.
   *
   * @param int $timestamp
   *   The RedHen Note creation timestamp.
   *
   * @return \Drupal\redhen_note\NoteInterface
   *   The called RedHen Note entity.
   */
  public function setCreatedTime($timestamp);

  /**
   * Returns the RedHen Note published status indicator.
   *
   * Unpublished RedHen Note are only visible to restricted users.
   *
   * @return bool
   *   TRUE if the RedHen Note is published.
   */
  public function isPublished();

  /**
   * Sets the published status of a RedHen Note.
   *
   * @param bool $published
   *   TRUE to set this RedHen Note to published, FALSE to set it to unpublished.
   *
   * @return \Drupal\redhen_note\NoteInterface
   *   The called RedHen Note entity.
   */
  public function setPublished($published);

}
