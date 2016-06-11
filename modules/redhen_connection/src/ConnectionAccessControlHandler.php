<?php

/**
 * @file
 * Contains \Drupal\redhen_connection\ConnectionAccessControlHandler.
 */

namespace Drupal\redhen_connection;

use Drupal\Core\Entity\EntityAccessControlHandler;
use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Session\AccountInterface;
use Drupal\Core\Access\AccessResult;

/**
 * Access controller for the Connection entity.
 *
 * @see \Drupal\redhen_connection\Entity\Connection.
 */
class ConnectionAccessControlHandler extends EntityAccessControlHandler {

  /**
   * {@inheritdoc}
   */
  protected $viewLabelOperation = TRUE;

  /**
   * {@inheritdoc}
   */
  protected function checkAccess(EntityInterface $entity, $operation, AccountInterface $account) {
    /** @var \Drupal\redhen_connection\ConnectionInterface $entity */

    switch ($operation) {
      // @todo split out view label into its own permission.
      case 'view label':
      case 'view':
        if (!$entity->isActive()) {
          return AccessResult::allowedIfHasPermission($account, 'view inactive Connection entities');
        }
        return AccessResult::allowedIfHasPermission($account, 'view active Connection entities');

      case 'update':
        return AccessResult::allowedIfHasPermission($account, 'edit Connection entities');

      case 'delete':
        return AccessResult::allowedIfHasPermission($account, 'delete Connection entities');
    }

    // Unknown operation, no opinion.
    return AccessResult::neutral();
  }

  /**
   * {@inheritdoc}
   */
  protected function checkCreateAccess(AccountInterface $account, array $context, $entity_bundle = NULL) {
    return AccessResult::allowedIfHasPermission($account, 'add Connection entities');
  }

}
