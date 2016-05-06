<?php
/**
 * @file
 * Contains \Drupal\redhen_relation\RelationPermissions.
 */


namespace Drupal\redhen_relation;

use Drupal\Core\StringTranslation\StringTranslationTrait;
use Drupal\redhen_relation\Entity\RelationType;

class RelationPermissions {
  
  use StringTranslationTrait;

  /**
   * Returns an array of RedHen Relation type permissions.
   *
   * @return array
   *    Returns an array of permissions.
   */
  public function RelationTypePermissions() {
    $perms = [];
    // Generate Relation permissions for all Relation types.
    foreach (RelationType::loadMultiple() as $type) {
      $perms += $this->buildPermissions($type);
    }

    return $perms;
  }

  /**
   * Builds a standard list of permissions for a given Relation type.
   *
   * @param \Drupal\redhen_relation\Entity\RelationType $relation_type
   *   The machine name of the Relation type.
   *
   * @return array
   *   An array of permission names and descriptions.
   */
  protected function buildPermissions(RelationType $relation_type) {
    $type_id = $relation_type->id();
    $type_params = ['%type' => $relation_type->label()];

    return [
      "add own $type_id Relation" => [
        'title' => $this->t('%type: Add own Relation', $type_params),
      ],
      "add any $type_id Relation" => [
        'title' => $this->t('%type: Add any Relation', $type_params),
      ],
      "view own $type_id Relation" => [
        'title' => $this->t('%type: View own Relation', $type_params),
      ],
      "view any $type_id Relation" => [
        'title' => $this->t('%type: View any Relation', $type_params),
      ],
      "edit own $type_id Relation" => [
        'title' => $this->t('%type: Edit own Relation', $type_params),
      ],
      "edit any $type_id Relation" => [
        'title' => $this->t('%type: Edit any Relation', $type_params),
      ],
      "delete own $type_id Relation" => [
        'title' => $this->t('%type: Delete own Relation', $type_params),
      ],
      "delete any $type_id Relation" => [
        'title' => $this->t('%type: Delete any Relation', $type_params),
      ],
    ];
  }

}
