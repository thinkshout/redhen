<?php

namespace Drupal\redhen_org;

use Drupal\Core\StringTranslation\StringTranslationTrait;
use Drupal\redhen_org\Entity\OrgType;

class OrgPermissions {
  
  use StringTranslationTrait;

  /**
   * Returns an array of RedHen org type permissions.
   *
   * @return array
   *    Returns an array of permissions.
   */
  public function OrgTypePermissions() {
    $perms = [];
    // Generate org permissions for all org types.
    foreach (OrgType::loadMultiple() as $type) {
      $perms += $this->buildPermissions($type);
    }

    return $perms;
  }

  /**
   * Builds a standard list of permissions for a given org type.
   *
   * @param \Drupal\redhen_org\Entity\OrgType $org_type
   *   The machine name of the org type.
   *
   * @return array
   *   An array of permission names and descriptions.
   */
  protected function buildPermissions(OrgType $org_type) {
    $type_id = $org_type->id();
    $type_params = ['%type' => $org_type->label()];

    return [
      "add $type_id org" => [
        'title' => $this->t('%type: Add org', $type_params),
      ],
      "view active $type_id org" => [
        'title' => $this->t('%type: View active orgs', $type_params),
      ],
      "view inactive $type_id org" => [
        'title' => $this->t('%type: View inactive orgs', $type_params),
      ],
      "edit $type_id org" => [
        'title' => $this->t('%type: Edit orgs', $type_params),
      ],
      "delete $type_id org" => [
        'title' => $this->t('%type: Delete orgs', $type_params),
      ],
    ];
  }

}
