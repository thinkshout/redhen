<?php
/**
 * @file
 * The controller for the membership entity containing the CRUD operations.
 */

/**
 * The controller class for memberships contains methods for the membership CRUD
 * operations. The load method is inherited from the default controller.
 */
class RedhenMembershipEntityController extends EntityAPIController {

  /**
   * Create a default membership.
   *
   * @param $type
   *   The machine-readable type of the membership.
   *
   * @return
   *   A membership object with all default fields initialized.
   */
  public function create(array $values = array()) {
    global $user;

    $values += array(
      'membership_id' => '',
      'label' => '',
      'author_uid' => $user->uid,
      'redhen_state' => REDHEN_STATE_ACTIVE,
      'created' => REQUEST_TIME,
      'changed' => REQUEST_TIME,
    );
    return parent::create($values);
  }

}
