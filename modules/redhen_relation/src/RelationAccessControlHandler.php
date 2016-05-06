<?php

/**
 * @file
 * Contains \Drupal\redhen_relation\RelationAccessControlHandler.
 */

namespace Drupal\redhen_relation;

use Drupal\Core\Entity\EntityAccessControlHandler;
use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Session\AccountInterface;
use Drupal\Core\Access\AccessResult;

/**
 * Access controller for the Relation entity.
 *
 * @see \Drupal\redhen_relation\Entity\Relation.
 */
class RelationAccessControlHandler extends EntityAccessControlHandler {
  /**
   * {@inheritdoc}
   */
  protected function checkAccess(EntityInterface $entity, $operation, AccountInterface $account) {
    /** @var \Drupal\redhen_relation\RelationInterface $entity */
    switch ($operation) {
      case 'view':
        if (!$entity->isActive()) {
          return AccessResult::allowedIfHasPermission($account, 'view inactive Relation entities');
        }
        return AccessResult::allowedIfHasPermission($account, 'view active Relation entities');

      case 'update':
        return AccessResult::allowedIfHasPermission($account, 'edit Relation entities');

      case 'delete':
        return AccessResult::allowedIfHasPermission($account, 'delete Relation entities');
    }

    // Unknown operation, no opinion.
    return AccessResult::neutral();
  }

  /**
   * {@inheritdoc}
   */
  protected function checkCreateAccess(AccountInterface $account, array $context, $entity_bundle = NULL) {
    return AccessResult::allowedIfHasPermission($account, 'add Relation entities');
  }

}
