<?php

/**
 * @file
 * Contains \Drupal\redhen_org\OrgAccessControlHandler.
 */

namespace Drupal\redhen_org;

use Drupal\Core\Entity\EntityAccessControlHandler;
use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Session\AccountInterface;
use Drupal\Core\Access\AccessResult;

/**
 * Access controller for the Org entity.
 *
 * @see \Drupal\redhen_org\Entity\Org.
 */
class OrgAccessControlHandler extends EntityAccessControlHandler {
  /**
   * {@inheritdoc}
   */
  protected function checkAccess(EntityInterface $entity, $operation, AccountInterface $account) {
    /** @var \Drupal\redhen_org\OrgInterface $entity */

    // Get Org bundle.
    $entity_bundle = $entity->getType();

    switch ($operation) {
      case 'view':
        // If Org is active, check "view active" permissions to determine
        // access.
        if ($entity->isActive()) {
          $view_access = AccessResult::allowedIfHasPermissions($account, [
            'view active org entities',
            'view active ' . $entity_bundle . ' org',
          ], 'OR');
        }
        // If Org is inactive, user needs "view inactive" permission to
        // view.
        else {
          $view_access = AccessResult::allowedIfHasPermissions($account, [
            'view inactive org entities',
            'view inactive ' . $entity_bundle . ' org',
          ], 'OR');
        }

        return $view_access;

      case 'update':
        return AccessResult::allowedIfHasPermission($account, 'edit org entities');

      case 'delete':
        return AccessResult::allowedIfHasPermission($account, 'delete org entities');
    }

    // Unknown operation, no opinion.
    return AccessResult::neutral();
  }

  /**
   * {@inheritdoc}
   */
  protected function checkCreateAccess(AccountInterface $account, array $context, $entity_bundle = NULL) {

    // If there is only one redhen_org bundle, set $entity_bundle to it
    // since OrgAddController::add returns the add form for the solitary
    // bundle instead of a bundle select form if there is only one.
    if (!$entity_bundle) {
      $types = \Drupal::entityTypeManager()->getStorage('redhen_org_type')->loadMultiple();
      if ($types && count($types) == 1) {
        $entity_bundle = array_keys($types)[0];
      }
    }
    return AccessResult::allowedIfHasPermissions($account, [
      'add org entities',
      'add ' . $entity_bundle . ' org',
    ], 'OR');
  }

}
