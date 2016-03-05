<?php
/**
 * @file
 * Contains \Drupal\redhen_contact\RedhenContactPermissions.
 */


namespace Drupal\redhen_contact;

use Drupal\Core\StringTranslation\StringTranslationTrait;
use Drupal\redhen_contact\Entity\RedhenContactType;

class RedhenContactPermissions {
  
  use StringTranslationTrait;

  /**
   * Returns an array of RedHen contact type permissions.
   *
   * @return array
   *    Returns an array of permissions.
   */
  public function redhenContactTypePermissions() {
    $perms = [];
    // Generate contact permissions for all contact types.
    foreach (RedhenContactType::loadMultiple() as $type) {
      $perms += $this->buildPermissions($type);
    }

    return $perms;
  }

  /**
   * Builds a standard list of permissions for a given contact type.
   *
   * @param \Drupal\redhen_contact\Entity\RedhenContactType $contact_type
   *   The machine name of the contact type.
   *
   * @return array
   *   An array of permission names and descriptions.
   */
  protected function buildPermissions(RedhenContactType $contact_type) {
    $type_id = $contact_type->id();
    $type_params = ['%type' => $contact_type->label()];

    return [
      "add own $type_id contact" => [
        'title' => $this->t('%type: Add own contact', $type_params),
      ],
      "add any $type_id contact" => [
        'title' => $this->t('%type: Add any contact', $type_params),
      ],
      "view own $type_id contact" => [
        'title' => $this->t('%type: View own contact', $type_params),
      ],
      "view any $type_id contact" => [
        'title' => $this->t('%type: View any contact', $type_params),
      ],
      "edit own $type_id contact" => [
        'title' => $this->t('%type: Edit own contact', $type_params),
      ],
      "edit any $type_id contact" => [
        'title' => $this->t('%type: Edit any contact', $type_params),
      ],
      "delete own $type_id contact" => [
        'title' => $this->t('%type: Delete own contact', $type_params),
      ],
      "delete any $type_id contact" => [
        'title' => $this->t('%type: Delete any contact', $type_params),
      ],
    ];
  }

}