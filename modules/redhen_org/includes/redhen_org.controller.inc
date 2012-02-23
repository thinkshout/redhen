<?php
/**
 * @file
 * The controller for the org entity containing the CRUD operations.
 */

/**
 * The controller class for orgs contains methods for the org CRUD
 * operations. The load method is inherited from the default controller.
 */
class RedhenOrgEntityController extends EntityAPIController {

  /**
   * Create a default org.
   *
   * @param $type
   *   The machine-readable type of the organization.
   *
   * @return
   *   A org object with all default fields initialized.
   */
  public function create(array $values = array()) {
    $values += array(
      'org_id' => '',
      'label' => '',
      'author_uid' => '',
      'primary_contact' => '',
      'redhen_state' => REDHEN_STATE_ACTIVE,
      'created' => '',
      'changed' => '',
    );
    return parent::create($values);
  }

}
