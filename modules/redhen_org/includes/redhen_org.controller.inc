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
   * @param $values
   *   Array of keyed values to use as defaults.
   *
   * @return
   *   A org object with all default fields initialized.
   */
  public function create(array $values = array()) {
    global $user;

    $values += array(
      'org_id' => '',
      'label' => '',
      'author_uid' => $user->uid,
      'primary_contact' => '',
      'redhen_state' => REDHEN_STATE_ACTIVE,
      'created' => REQUEST_TIME,
      'changed' => REQUEST_TIME,
    );
    return parent::create($values);
  }

}
