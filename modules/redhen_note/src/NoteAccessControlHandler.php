<?php

namespace Drupal\redhen_note;

use Drupal\Core\Entity\EntityAccessControlHandler;
use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Session\AccountInterface;
use Drupal\Core\Access\AccessResult;

/**
 * Access controller for the RedHen Note entity.
 *
 * @see \Drupal\redhen_note\Entity\Note.
 */
class NoteAccessControlHandler extends EntityAccessControlHandler {
  /**
   * {@inheritdoc}
   */
  protected function checkAccess(EntityInterface $entity, $operation, AccountInterface $account) {
    /** @var \Drupal\redhen_note\NoteInterface $entity */
    switch ($operation) {
      case 'view':
        if (!$entity->isPublished()) {
          return AccessResult::allowedIfHasPermission($account, 'view unpublished redhen note entities');
        }
        return AccessResult::allowedIfHasPermission($account, 'view published redhen note entities');

      case 'update':
        return AccessResult::allowedIfHasPermission($account, 'edit redhen note entities');

      case 'delete':
        return AccessResult::allowedIfHasPermission($account, 'delete redhen note entities');
    }

    // Unknown operation, no opinion.
    return AccessResult::neutral();
  }

  /**
   * {@inheritdoc}
   */
  protected function checkCreateAccess(AccountInterface $account, array $context, $entity_bundle = NULL) {
    return AccessResult::allowedIfHasPermission($account, 'add redhen note entities');
  }

}
