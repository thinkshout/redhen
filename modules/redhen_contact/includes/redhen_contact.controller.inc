<?php
/**
 * @file
 * The controller for the contact entity containing the CRUD operations.
 */

/**
 * The controller class for contacts contains methods for the contact CRUD
 * operations. The load method is inherited from the default controller.
 */
class RedhenContactEntityController extends EntityAPIController {

  /**
   * Create a default contact.
   *
   * @param $type
   *   The machine-readable type of the contact.
   *
   * @return
   *   A contact object with all default fields initialized.
   */
  public function create(array $values = array()) {
    $values += array(
      'contact_id' => '',
      'revision_id' => '',
      'first_name' => '',
      'last_name' => '',
      'user_uid' => '',
      'author_uid' => '',
      'redhen_state' => REDHEN_STATE_ACTIVE,
      'created' => '',
      'changed' => '',
    );
    return parent::create($values);
  }

  /**
   * Saves a contact.
   *
   * @param $contact
   *   The full contact object to save.
   *
   * @return
   *   The saved contact object.
   */
  public function save($contact) {
    $transaction = db_transaction();

    try {
      $contact->updated = REQUEST_TIME;

      // Give modules the opportunity to prepare field data for saving.
      field_attach_presave('redhen_contact', $contact);

      // If this is a new contact...
      if ((isset($contact->is_new) && $contact->is_new) || empty($contact->contact_id)) {
        // Set the creation timestamp if not set.
        if (!isset($contact->created) || empty($contact->created)) {
          $contact->created = REQUEST_TIME;
        }

        // Save the new contact and fields.
        drupal_write_record('redhen_contact', $contact);
        drupal_write_record('redhen_contact_revision', $contact);
        field_attach_insert('redhen_contact', $contact);

        $op = 'insert';
      }
      else {
        if (isset($contact->revision_id)) {
          // Save the old revision ID in case we need it in alters.
          $contact->old_revision_id = $contact->revision_id;
          // Unset the revision_id, it'll get updated when we insert into the
          // {redhen_contact_revision} table.
          unset($contact->revision_id);
        }

        // Save the updated contact and fields.
        drupal_write_record('redhen_contact', $contact, 'contact_id');
        drupal_write_record('redhen_contact_revision', $contact);
        field_attach_update('redhen_contact', $contact);

        $op = 'update';
      }
      // Since we always save a new revision, update the vid
      db_update('redhen_contact')
        ->fields(array('revision_id' => $contact->revision_id))
        ->condition('contact_id', $contact->contact_id)
        ->execute();


      module_invoke_all('redhen_contact_' . $op, $contact);
      module_invoke_all('entity_' . $op, $contact, 'redhen_contact');

      // Ignore slave server temporarily to give time for the
      // saved contact to be propagated to the slave.
      db_ignore_slave();

      return $contact;
    }
    catch (Exception $e) {
      $transaction->rollback();
      watchdog_exception('redhen_contact', $e);
      throw $e;
    }
  }

  /**
   * Deletes multiple contacts by ID.
   *
   * @param $contact_ids
   *   An array of contact IDs to delete.
   *
   * @return
   *   TRUE on success, FALSE otherwise.
   */
  public function delete($contact_ids) {
    if (!empty($contact_ids)) {
      $contacts = $this->load($contact_ids, array());

      // Ensure the contacts can actually be deleted.
      foreach ((array) $contacts as $contact_id => $contact) {
        if (in_array(FALSE, module_invoke_all('redhen_contact_can_delete', $contact))) {
          unset($contacts[$contact_id]);
        }
      }

      // If none of the specified contacts can be deleted, return FALSE.
      if (empty($contacts)) {
        return FALSE;
      }

      $transaction = db_transaction();

      try {
        db_delete('redhen_contact')
          ->condition('contact_id', $contact_ids, 'IN')
          ->execute();

        db_delete('redhen_contact_revision')
          ->condition('contact_id', $contact_ids, 'IN')
          ->execute();

        foreach ($contacts as $contact_id => $contact) {
          module_invoke_all('redhen_contact_delete', $contact);
          field_attach_delete('redhen_contact', $contact);
        }

        // Ignore slave server temporarily to give time for the
        // saved contact to be propagated to the slave.
        db_ignore_slave();
      }
      catch (Exception $e) {
        $transaction->rollback();
        watchdog_exception('redhen_contact', $e);
        throw $e;
      }

      // Clear the page and block and contact_load_multiple caches.
      cache_clear_all();
      $this->resetCache();
    }

    return TRUE;
  }

}
