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
   * Saves a membership.
   *
   * @param $membership
   *   The full membership object to save.
   *
   * @return
   *   The saved membership object.
   */
  public function save($membership) {
    $transaction = db_transaction();
    try {
      $membership->updated = REQUEST_TIME;

      parent::save($membership, $transaction);

      // Toggle user roles as needed after saving the membership
      redhen_membership_toggle_roles($membership);

      return $membership;
    }
    catch (Exception $e) {
      $transaction->rollback();
      watchdog_exception('redhen_membership', $e);
      throw $e;
    }
  }

  /**
   * Deletes a membership.
   *
   * @param $membership
   *   The full membership object to delete.
   */
  public function delete($membership) {
    $transaction = db_transaction();
    try {
      // explicitly set the state to archived to trigger removal of roles
      $membership->redhen_state = REDHEN_STATE_ARCHIVED;
      redhen_membership_toggle_roles($membership);

      parent::delete(array($membership->membership_id), $transaction);
    }
    catch (Exception $e) {
      $transaction->rollback();
      watchdog_exception('redhen_membership', $e);
      throw $e;
    }
  }

}
