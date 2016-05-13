<?php
/**
 * @file
 * Contains \Drupal\redhen_connection\ConnectionPermissions.
 */


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
      "add own $type_id Connection" => [
        'title' => $this->t('%type: Add own Connection', $type_params),
      ],
      "add any $type_id Connection" => [
        'title' => $this->t('%type: Add any Connection', $type_params),
      ],
      "view own $type_id Connection" => [
        'title' => $this->t('%type: View own Connection', $type_params),
      ],
      "view any $type_id Connection" => [
        'title' => $this->t('%type: View any Connection', $type_params),
      ],
      "edit own $type_id Connection" => [
        'title' => $this->t('%type: Edit own Connection', $type_params),
      ],
      "edit any $type_id Connection" => [
        'title' => $this->t('%type: Edit any Connection', $type_params),
      ],
      "delete own $type_id Connection" => [
        'title' => $this->t('%type: Delete own Connection', $type_params),
      ],
      "delete any $type_id Connection" => [
        'title' => $this->t('%type: Delete any Connection', $type_params),
      ],
    ];
  }

}
