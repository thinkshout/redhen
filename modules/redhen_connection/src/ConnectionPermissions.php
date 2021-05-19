<?php

namespace Drupal\redhen_connection;

use Drupal\Core\StringTranslation\StringTranslationTrait;
use Drupal\redhen_connection\Entity\ConnectionType;

class ConnectionPermissions {
  
  use StringTranslationTrait;

  /**
   * Returns an array of RedHen Connection type permissions.
   *
   * @return array
   *    Returns an array of permissions.
   */
  public function ConnectionTypePermissions() {
    $perms = [];
    // Generate Connection permissions for all Connection types.
    foreach (ConnectionType::loadMultiple() as $type) {
      $perms += $this->buildPermissions($type);
    }
    $perms += $this->buildListingPermissions();

    return $perms;
  }

  /**
   * Builds a standard list of permissions for a given Connection type.
   *
   * @param \Drupal\redhen_connection\Entity\ConnectionType $connection_type
   *   The machine name of the Connection type.
   *
   * @return array
   *   An array of permission names and descriptions.
   */
  protected function buildPermissions(ConnectionType $connection_type) {
    $type_id = $connection_type->id();
    $type_params = ['%type' => $connection_type->label()];

    return [
      "add $type_id connection" => [
        'title' => $this->t('%type: Add connection', $type_params),
      ],
      "view active $type_id connection" => [
        'title' => $this->t('%type: View active connections', $type_params),
      ],
      "view inactive $type_id connection" => [
        'title' => $this->t('%type: View inactive connections', $type_params),
      ],
      "edit $type_id connection" => [
        'title' => $this->t('%type: Edit connections', $type_params),
      ],
      "delete $type_id connection" => [
        'title' => $this->t('%type: Delete connections', $type_params),
      ],
    ];
  }

  /**
   * Builds a list of permissions for a the Connections tab per entity.
   *
   * @return array
   *   An array of permission names and descriptions.
   */
  protected function buildListingPermissions() {
    $permissions = [];
    foreach (\Drupal::service('redhen_connection.connections')->getAllConnectionEntityTypes() as $entity_type_id => $entity_type) {
      // If the entity didn't get a redhen_connection link template added by
      // hook_entity_types_alter(), skip it.
      if (!($path = $entity_type->getLinkTemplate('redhen_connection'))) {
        continue;
      }

      $permissions["view own active $entity_type_id connection"] = [
        'title' => $this->t('%type: View own active connections', ['%type' => $entity_type->getLabel()]),
      ];
    }

    return $permissions;
  }

}
