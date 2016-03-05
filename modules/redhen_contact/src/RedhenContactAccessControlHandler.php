<?php

/**
 * @file
 * Contains \Drupal\redhen_contact\RedhenContactAccessControlHandler.
 */

namespace Drupal\redhen_contact;

use Drupal\Core\Entity\EntityAccessControlHandler;
use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Session\AccountInterface;
use Drupal\Core\Access\AccessResult;

/**
 * Access controller for the Contact entity.
 *
 * @see \Drupal\redhen_contact\Entity\RedhenContact.
 */
class RedhenContactAccessControlHandler extends EntityAccessControlHandler {
  /**
   * {@inheritdoc}
   */
  protected function checkAccess(EntityInterface $entity, $operation, AccountInterface $account) {
    /** @var \Drupal\redhen_contact\RedhenContactInterface $entity */
    switch ($operation) {
      case 'view':
        if (!$entity->isActive()) {
          return AccessResult::allowedIfHasPermission($account, 'view inactive contact entities');
        }
        return AccessResult::allowedIfHasPermission($account, 'view active contact entities');

      case 'update':
        return AccessResult::allowedIfHasPermission($account, 'edit contact entities');

      case 'delete':
        return AccessResult::allowedIfHasPermission($account, 'delete contact entities');
    }

    // Unknown operation, no opinion.
    return AccessResult::neutral();
  }

  /**
   * {@inheritdoc}
   */
  protected function checkCreateAccess(AccountInterface $account, array $context, $entity_bundle = NULL) {
    return AccessResult::allowedIfHasPermission($account, 'add contact entities');
  }

}
