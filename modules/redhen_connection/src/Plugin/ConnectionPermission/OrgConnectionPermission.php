<?php

namespace Drupal\redhen_connection\Plugin\ConnectionPermission;

use Drupal\Core\Access\AccessResultAllowed;
use Drupal\redhen_connection\ConnectionInterface;
use Drupal\redhen_connection\Plugin\ConnectionPermissionBase;
use Drupal\redhen_connection\Plugin\ConnectionPermissionInterface;
use Drupal\Core\Entity\EntityInterface;
use Drupal\redhen_contact\Entity\Contact;
use Drupal\Core\Plugin\ContainerFactoryPluginInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * @ConnectionPermission(
 *  id = "org_connection_permission",
 *  label = @Translation("Organization"),
 *  description = @Translation("Sitewide permissions will override this setting."),
 *  subject_entity_type = "redhen_org",
 *  subject_entity_bundle = "",
 *  influencer_entity_type = "",
 * )
 */
class OrgConnectionPermission extends ConnectionPermissionBase implements ConnectionPermissionInterface {


  public function getPermissionKey() {
    return 'org';
  }

  /**
   * {@inheritdoc}
   */
  public function getInfluencer(EntityInterface $subject_entity) {}

  /**
   * {@inheritdoc}
   */
  public function hasRolePermissions(EntityInterface $subject_entity, $operation, Contact $contact) {
    // @todo how to include the service in a more abstract way?
    $redhenConnectionConnections = \Drupal::service('redhen_connection.connections');
    return $redhenConnectionConnections->checkConnectionPermission($contact, $subject_entity, $operation, $this->getPermissionKey());
  }
}
