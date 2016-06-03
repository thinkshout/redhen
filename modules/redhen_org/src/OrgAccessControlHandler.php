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
    switch ($operation) {
      case 'view':
        if (!$entity->isActive()) {
          return AccessResult::allowedIfHasPermission($account, 'view inactive org entities');
        }
        return AccessResult::allowedIfHasPermission($account, 'view active org entities');

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
    return AccessResult::allowedIfHasPermission($account, 'add org entities');
  }

}
