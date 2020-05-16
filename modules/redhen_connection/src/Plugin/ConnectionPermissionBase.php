<?php

namespace Drupal\redhen_connection\Plugin;

use Drupal\Component\Plugin\PluginBase;
use Drupal\Core\Entity\EntityInterface;
use Drupal\redhen_contact\Entity\Contact;

/**
 * Base class for Connection permission plugins.
 */
class ConnectionPermissionBase extends PluginBase implements ConnectionPermissionInterface {


  /**
   * @inheritDoc
   */
  public function getPermissionKey() {
    return $this->get('subject_entity_type') . ($this->get('subject_entity_bundle') ? ".{$this->get('subject_entity_bundle')}" : "");
  }

  /**
   * @inheritDoc
   */
  public function getInfluencer(EntityInterface $subject_entity) {}

  /**
   * @inheritDoc
   */
  public function hasRolePermissions(EntityInterface $subject_entity, $operation, Contact $contact) {}

  /**
   * Get values from plugin definition.
   * @param $value
   * @return mixed
   */
  public function get($value) {
    return $this->pluginDefinition[$value];
  }

}
