<?php

namespace Drupal\redhen_contact;

use Drupal\Core\Entity\EntityAccessControlHandler;
use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Session\AccountInterface;
use Drupal\Core\Access\AccessResult;

/**
 * Access controller for the Contact entity.
 *
 * @see \Drupal\redhen_contact\Entity\Contact.
 */
class ContactAccessControlHandler extends EntityAccessControlHandler {

  /**
   * {@inheritdoc}
   */
  protected $viewLabelOperation = TRUE;

  /**
   * {@inheritdoc}
   */
  protected function checkAccess(EntityInterface $entity, $operation, AccountInterface $account) {
    /** @var \Drupal\redhen_contact\ContactInterface $entity */

    // Get Contact bundle.
    $entity_bundle = $entity->getType();

    // Check if Contact being accessed is user's own.
    $own = $entity->getOwnerId() == $account->id();

    switch ($operation) {
      // @todo split out view label into its own permission.
      case 'view label':
      case 'view':
        // If Contact is active, check "view own" and/or "view active"
        // permissions to determine access.
        if ($entity->isActive()) {
          // If Contact is user's own, either "view active" or "view own"
          // permission is sufficient to grant access.
          if ($own) {
            $view_access = AccessResult::allowedIfHasPermissions($account, [
              'view active contact entities',
              'view active ' . $entity_bundle . ' contact',
              'view own ' . $entity_bundle . ' contact',
            ], 'OR');
          }
          // If Contact is not user's own, user needs "view active" permission
          // to view.
          else {
            $view_access = AccessResult::allowedIfHasPermissions($account, [
              'view active contact entities',
              'view active ' . $entity_bundle . ' contact',
            ], 'OR');
          }
        }
        // If Contact is inactive, user needs "view inactive" permission to
        // view.
        else {
          $view_access = AccessResult::allowedIfHasPermissions($account, [
            'view inactive contact entities',
            'view inactive ' . $entity_bundle . ' contact',
          ], 'OR');
        }

        return $view_access;

      case 'update':
        // If Contact is active, check "edit own" and/or "edit active"
        // permissions to determine access.
        if ($entity->isActive()) {
          // If Contact is user's own, either "edit any" or "edit own"
          // permission is sufficient to grant access.
          if ($own) {
            $edit_access = AccessResult::allowedIfHasPermissions($account, [
              'edit contact entities',
              'edit any ' . $entity_bundle . ' contact',
              'edit own ' . $entity_bundle . ' contact',
            ], 'OR');
          }
        }
        // If Contact is inactive or not user's own, user needs "edit any"
        // permission to have access.
        if (!isset($edit_access)) {
          $edit_access = AccessResult::allowedIfHasPermissions($account, [
            'edit contact entities',
            'edit any ' . $entity_bundle . ' contact',
          ], 'OR');
        }

        return $edit_access;

      case 'delete':
        // If Contact is active, check "delete own" and/or "delete active"
        // permissions to determine access.
        if ($entity->isActive()) {
          // If Contact is user's own, either "delete any" or "delete own"
          // permission is sufficient to grant access.
          if ($own) {
            $delete_access = AccessResult::allowedIfHasPermissions($account, [
              'delete contact entities',
              'delete any ' . $entity_bundle . ' contact',
              'delete own ' . $entity_bundle . ' contact',
            ], 'OR');
          }
        }
        // If Contact is inactive or not user's own, user needs "delete any"
        // permission to have access.
        if (!isset($delete_access)) {
          $delete_access = AccessResult::allowedIfHasPermissions($account, [
            'delete contact entities',
            'delete any ' . $entity_bundle . ' contact',
          ], 'OR');
        }

        return $delete_access;
    }

    // Unknown operation, no opinion.
    return AccessResult::neutral();
  }

  /**
   * {@inheritdoc}
   */
  protected function checkCreateAccess(AccountInterface $account, array $context, $entity_bundle = NULL) {

    // If there is only one redhen_contact bundle, set $entity_bundle to it
    // since ContactAddController::add returns the add form for the solitary
    // bundle instead of a bundle select form if there is only one.
    if (!$entity_bundle) {
      $types = \Drupal::entityTypeManager()->getStorage('redhen_contact_type')->loadMultiple();
      if ($types && count($types) == 1) {
        $entity_bundle = array_keys($types)[0];
      }
    }
    return AccessResult::allowedIfHasPermissions($account, [
      'add contact entities',
      'add ' . $entity_bundle . ' contact',
    ], 'OR');
  }

}
