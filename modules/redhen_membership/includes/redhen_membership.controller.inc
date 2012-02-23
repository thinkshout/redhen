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
    $values += array(
      'membership_id' => '',
      'label' => '',
      'author_uid' => '',
      'redhen_state' => REDHEN_STATE_ACTIVE,
      'created' => '',
      'changed' => '',
    );
    return parent::create($values);
  }

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

      // Give modules the opportunity to prepare field data for saving.
      field_attach_presave('redhen_membership', $membership);

      // If this is a new membership...
      if ((isset($membership->is_new) && $membership->is_new) || empty($membership->membership_id)) {
        // Set the creation timestamp if not set.
        if (!isset($membership->created) || empty($membership->created)) {
          $membership->created = REQUEST_TIME;
        }

        // Save the new membership and fields.
        drupal_write_record('redhen_membership', $membership);
        field_attach_insert('redhen_membership', $membership);

        $op = 'insert';
      }
      else {
        // Save the updated membership and fields.
        drupal_write_record('redhen_membership', $membership, 'membership_id');
        field_attach_update('redhen_membership', $membership);

        $op = 'update';
      }


      module_invoke_all('redhen_membership_' . $op, $membership);
      module_invoke_all('entity_' . $op, $membership, 'redhen_membership');

      // Ignore slave server temporarily to give time for the
      // saved membership to be propagated to the slave.
      db_ignore_slave();

      return $membership;
    }
    catch (Exception $e) {
      $transaction->rollback();
      watchdog_exception('redhen_membership', $e);
      throw $e;
    }
  }

  /**
   * Deletes multiple memberships by ID.
   *
   * @param $membership_ids
   *   An array of membership IDs to delete.
   *
   * @return
   *   TRUE on success, FALSE otherwise.
   */
  public function delete($membership_ids) {
    if (!empty($membership_ids)) {
      $memberships = $this->load($membership_ids, array());

      // Ensure the memberships can actually be deleted.
      foreach ((array) $memberships as $membership_id => $membership) {
        if (in_array(FALSE, module_invoke_all('redhen_membership_can_delete', $membership))) {
          unset($memberships[$membership_id]);
        }
      }

      // If none of the specified memberships can be deleted, return FALSE.
      if (empty($memberships)) {
        return FALSE;
      }

      $transaction = db_transaction();

      try {
        db_delete('redhen_membership')
          ->condition('membership_id', $membership_ids, 'IN')
          ->execute();

        foreach ($memberships as $membership_id => $membership) {
          module_invoke_all('redhen_membership_delete', $membership);
          field_attach_delete('redhen_membership', $membership);
        }

        // Ignore slave server temporarily to give time for the
        // saved membership to be propagated to the slave.
        db_ignore_slave();
      }
      catch (Exception $e) {
        $transaction->rollback();
        watchdog_exception('redhen_membership', $e);
        throw $e;
      }

      // Clear the page and block and membership_load_multiple caches.
      cache_clear_all();
      $this->resetCache();
    }

    return TRUE;
  }
}
