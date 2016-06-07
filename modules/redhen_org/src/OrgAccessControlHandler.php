<?php

/**
 * @file
 * Contains \Drupal\redhen_org\OrgAccessControlHandler.
 */

namespace Drupal\redhen_org;

use Drupal\Core\Entity\EntityAccessControlHandler;
use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Entity\EntityTypeInterface;
use Drupal\Core\Session\AccountInterface;
use Drupal\Core\Access\AccessResult;
use Drupal\redhen_connection\ConnectionServiceInterface;
use Drupal\redhen_connection\Access\ConnectionAccessResult;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Drupal\Core\Entity\EntityHandlerInterface;

/**
 * Access controller for the Org entity.
 *
 * @see \Drupal\redhen_org\Entity\Org.
 */
class OrgAccessControlHandler extends EntityAccessControlHandler implements EntityHandlerInterface {


  /**
   * The Connection service.
   *
   * @var \Drupal\redhen_connection\ConnectionServiceInterface
   */
  protected $connections;

  /**
   * Constructs a OrgAccessControlHandler object.
   *
   * @param \Drupal\Core\Entity\EntityTypeInterface $entity_type
   *   The entity type definition.
   * @param \Drupal\redhen_connection\ConnectionServiceInterface $connections
   *   The node grant storage.
   */
  public function __construct(EntityTypeInterface $entity_type, ConnectionServiceInterface $connections) {
    parent::__construct($entity_type);
    $this->connections = $connections;
  }

  /**
   * {@inheritdoc}
   */
  public static function createInstance(ContainerInterface $container, EntityTypeInterface $entity_type) {
    return new static(
      $entity_type,
      $container->get('redhen_connection.connections')
    );
  }

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
        // Check admin and bundle-specific edit permissions to determine
        // edit access.
        $edit_access = AccessResult::allowedIfHasPermissions($account, [
          'edit org entities',
          'edit any ' . $entity_bundle . ' org',
        ], 'OR');

        return $edit_access;

      case 'delete':
        // Check admin and bundle-specific delete permissions to determine
        // delete access.
        $delete_access = AccessResult::allowedIfHasPermissions($account, [
          'delete org entities',
          'delete any ' . $entity_bundle . ' org',
        ], 'OR');

        return $delete_access;
    }

    return ConnectionAccessResult::allowedIfHasConnectionPermission($entity, $account, $operation);

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
