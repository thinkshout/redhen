<?php

/**
 * @file
 * Contains \Drupal\redhen_org\OrgAccessControlHandler.
 */

namespace Drupal\redhen_org;

use Drupal\Core\Entity\EntityAccessControlHandler;
use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Entity\ContentEntityTypeInterface;
use Drupal\Core\Session\AccountInterface;
use Drupal\Core\Access\AccessResult;
use Drupal\redhen_connection\ConnectionServiceInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Access controller for the Org entity.
 *
 * @see \Drupal\redhen_org\Entity\Org.
 */
class OrgAccessControlHandler extends EntityAccessControlHandler {


  /**
   * The Connection service.
   *
   * @var \Drupal\redhen_connection\ConnectionServiceInterface
   */
  protected $connectionService;

  /**
   * Constructs a OrgAccessControlHandler object.
   *
   * @param \Drupal\Core\Entity\ContentEntityTypeInterface $entity_type
   *   The entity type definition.
   * @param \Drupal\redhen_connection\ConnectionServiceInterface $connectionService
   *   The node grant storage.
   */
  public function __construct(ContentEntityTypeInterface $entity_type, ConnectionServiceInterface $connectionService) {
    parent::__construct($entity_type);
    $this->connectionService = $connectionService;
  }

  /**
   * {@inheritdoc}
   */
  public static function createInstance(ContainerInterface $container, ContentEntityTypeInterface $entity_type) {
    return new static(
      $entity_type,
      $container->get('redhen_connection')
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
