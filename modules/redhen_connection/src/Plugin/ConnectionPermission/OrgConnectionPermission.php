<?php

namespace Drupal\redhen_connection\Plugin\ConnectionPermission;

use Drupal\redhen_connection\Plugin\ConnectionPermissionBase;
use Drupal\redhen_connection\Plugin\ConnectionPermissionInterface;
use Drupal\Core\Entity\EntityInterface;
use Drupal\redhen_contact\Entity\Contact;

/**
 * Provides permission for access to connected redhen_orgs.
 *
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

  /**
   * {@inheritDoc}
   */
  public function getPermissionKey() {
    return 'entity';
  }

  /**
   * {@inheritdoc}
   */
  public function hasRolePermissions(EntityInterface $subject_entity, $operation, Contact $contact) {
    return $this->redhenConnectionConnections->checkConnectionPermission($contact, $subject_entity, $operation, $this->getPermissionKey());
  }

}
