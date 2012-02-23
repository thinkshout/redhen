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
   *   The machine-readable type of the org.
   *
   * @return
   *   A org object with all default fields initialized.
   */
  public function create(array $values = array()) {
    $values += array(
      'org_id' => '',
      'label' => '',
      'author_uid' => '',
      'redhen_state' => REDHEN_STATE_ACTIVE,
      'created' => '',
      'changed' => '',
    );
    return parent::create($values);
  }

  /**
   * Saves an org.
   *
   * @param $org
   *   The full org object to save.
   *
   * @return
   *   The saved org object.
   */
  public function save($org) {
    $transaction = db_transaction();

    try {
      $org->updated = REQUEST_TIME;

      // Give modules the opportunity to prepare field data for saving.
      field_attach_presave('redhen_org', $org);

      // If this is a new org...
      if ((isset($org->is_new) && $org->is_new) || empty($org->org_id)) {
        // Set the creation timestamp if not set.
        if (!isset($org->created) || empty($org->created)) {
          $org->created = REQUEST_TIME;
        }

        // Save the new org and fields.
        drupal_write_record('redhen_org', $org);
        field_attach_insert('redhen_org', $org);

        $op = 'insert';
      }
      else {
        // Save the updated org and fields.
        drupal_write_record('redhen_org', $org, 'org_id');
        field_attach_update('redhen_org', $org);

        $op = 'update';
      }


      module_invoke_all('redhen_org_' . $op, $org);
      module_invoke_all('entity_' . $op, $org, 'redhen_org');

      // Ignore slave server temporarily to give time for the
      // saved org to be propagated to the slave.
      db_ignore_slave();

      return $org;
    }
    catch (Exception $e) {
      $transaction->rollback();
      watchdog_exception('redhen_org', $e);
      throw $e;
    }
  }

  /**
   * Deletes multiple orgs by ID.
   *
   * @param $org_ids
   *   An array of org IDs to delete.
   *
   * @return
   *   TRUE on success, FALSE otherwise.
   */
  public function delete($org_ids) {
    if (!empty($org_ids)) {
      $orgs = $this->load($org_ids, array());

      // Ensure the orgs can actually be deleted.
      foreach ((array) $orgs as $org_id => $org) {
        if (in_array(FALSE, module_invoke_all('redhen_org_can_delete', $org))) {
          unset($orgs[$org_id]);
        }
      }

      // If none of the specified orgs can be deleted, return FALSE.
      if (empty($orgs)) {
        return FALSE;
      }

      $transaction = db_transaction();

      try {
        db_delete('redhen_org')
          ->condition('org_id', $org_ids, 'IN')
          ->execute();

        foreach ($orgs as $org_id => $org) {
          module_invoke_all('redhen_org_delete', $org);
          field_attach_delete('redhen_org', $org);
        }

        // Ignore slave server temporarily to give time for the
        // saved org to be propagated to the slave.
        db_ignore_slave();
      }
      catch (Exception $e) {
        $transaction->rollback();
        watchdog_exception('redhen_org', $e);
        throw $e;
      }

      // Clear the page and block and org_load_multiple caches.
      cache_clear_all();
      $this->resetCache();
    }

    return TRUE;
  }
}
