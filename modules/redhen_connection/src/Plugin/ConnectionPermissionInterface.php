<?php

namespace Drupal\redhen_connection\Plugin;

use Drupal\Component\Plugin\PluginInspectionInterface;
use Drupal\Core\Entity\EntityInterface;
use Drupal\redhen_contact\Entity\Contact;

/**
 * Defines an interface for Connection permission plugins.
 */
interface ConnectionPermissionInterface extends PluginInspectionInterface {

  /**
   * Get the entity key.
   *
   * @return string
   *   String representation of entity type for the permissions form.
   */
  public function getPermissionKey();

  /**
   * Get the influencer entity.
   *
   * @param \Drupal\Core\Entity\EntityInterface $subject_entity
   *   The subject entity.
   *
   * @return array
   *   An array of Entities that influence the access of the subject entity.
   */
  public function getInfluencers(EntityInterface $subject_entity);

  /**
   * Determine if the contact execute the operation on the subject entity.
   *
   * @param \Drupal\Core\Entity\EntityInterface $subject_entity
   *   The entity that is being accessed.
   * @param string $operation
   *   The operation that is being performed (view, update, delete, view label).
   * @param \Drupal\redhen_contact\Entity\Contact $contact
   *   The Redhen contact object.
   *
   * @return bool
   *   True is access is allowed, false if neutral.
   */
  public function hasRolePermissions(EntityInterface $subject_entity, $operation, Contact $contact);

}
