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

      parent::save($contact, $transaction);

      // If this is a new contact...
      if (!empty($contact->contact_id)) {
        if (isset($contact->revision_id)) {
          // Save the old revision ID in case we need it in alters.
          $contact->old_revision_id = $contact->revision_id;
          // Unset the revision_id, it'll get updated when we insert into the
          // {redhen_contact_revision} table.
          unset($contact->revision_id);
        }
      }

      // create revision
      drupal_write_record('redhen_contact_revision', $contact);

      // Since we always save a new revision, update the vid
      db_update('redhen_contact')
        ->fields(array('revision_id' => $contact->revision_id))
        ->condition('contact_id', $contact->contact_id)
        ->execute();

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
        parent::delete($contact_ids, $transaction);

        db_delete('redhen_contact_revision')
          ->condition('contact_id', $contact_ids, 'IN')
          ->execute();
      }
      catch (Exception $e) {
        $transaction->rollback();
        watchdog_exception('redhen_contact', $e);
        throw $e;
      }
    }

    return TRUE;
  }

}
