<?php

namespace Drupal\redhen_connection\Access;

use Drupal\Core\Access\AccessResult;
use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Session\AccountInterface;
use Drupal\redhen_connection\ConnectionServiceInterface;


/**
 * Extends the AccessResult class with connection permission checks.
 */
abstract class ConnectionAccessResult extends AccessResult {

  /**
   * Allows access if the permission is present, neutral otherwise.
   *
   * @todo Potentially cache this based on https://www.drupal.org/node/2667018.
   *
   * @param \Drupal\Core\Entity\EntityInterface $entity
   *   The entity for which to check a permission.
   * @param \Drupal\Core\Session\AccountInterface $account
   *   The account for which to check a permission.
   * @param string $permission
   *   The permission to check for.
   *
   * @return \Drupal\Core\Access\AccessResult
   *   If the account has the permission, isAllowed() will be TRUE, otherwise
   *   isNeutral() will be TRUE.
   */
  public static function allowedIfHasConnectionPermission(EntityInterface $entity, AccountInterface $account, $permission) {
    /** @var ConnectionServiceInterface $connection_service */
    $connection_service = \Drupal::service('redhen_connection.connections');
    return static::allowedIf($connection_service->checkConnectionPermission($entity, $permission, $account));
  }

  /**
   * Allows access if the permissions are present, neutral otherwise.
   *
   * @todo Potentially cache this based on https://www.drupal.org/node/2667018.
   *
   * @param \Drupal\Core\Entity\EntityInterface $entity
   *   The entity for which to check permissions.
   * @param \Drupal\Core\Session\AccountInterface $account
   *   The account for which to check permissions.
   * @param array $permissions
   *   The permissions to check.
   * @param string $conjunction
   *   (optional) 'AND' if all permissions are required, 'OR' in case just one.
   *   Defaults to 'AND'.
   *
   * @return \Drupal\Core\Access\AccessResult
   *   If the account has the permissions, isAllowed() will be TRUE, otherwise
   *   isNeutral() will be TRUE.
   */
  public static function allowedIfHasConnectionPermissions(EntityInterface $entity, AccountInterface $account, array $permissions, $conjunction = 'AND') {
    $access = FALSE;

    if ($conjunction == 'AND' && !empty($permissions)) {
      $access = TRUE;
      foreach ($permissions as $permission) {
        if (!$permission_access = $entity->hasPermission($permission, $account)) {
          $access = FALSE;
          break;
        }
      }
    }
    else {
      foreach ($permissions as $permission) {
        if ($permission_access = $entity->hasPermission($permission, $account)) {
          $access = TRUE;
          break;
        }
      }
    }

    return static::allowedIf($access);
  }

}