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

      // new contact, set created prop
      if (isset($contact->is_new) && $contact->is_new) {
        $contact->created = REQUEST_TIME;
      }
      // handle revision id
      else {
        if (isset($contact->revision_id)) {
          // Save the old revision ID in case we need it in alters.
          $contact->old_revision_id = $contact->revision_id;
          // Unset the revision_id, it'll get updated when we insert into the
          // {redhen_contact_revision} table.
          unset($contact->revision_id);
        }
      }

      parent::save($contact, $transaction);

      // create revision
      drupal_write_record('redhen_contact_revision', $contact);

      // Since we always save a new revision, update the vid
      db_update('redhen_contact')
        ->fields(array('revision_id' => $contact->revision_id))
        ->condition('contact_id', $contact->contact_id)
        ->execute();

      // set the contact user
      if ($contact->user_uid) {
        $this->setUser($contact);
      }

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
        } elseif (isset($contact->user)) {
          module_invoke_all('redhen_contact_user_remove', TRUE, $contact);
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

        // delete user connections
        db_delete('redhen_contact_user')
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

  /**
   * Override buildQuery to add a join for the active Drupal user.
   *
   * @param $ids
   * @param array $conditions
   * @param bool $revision_id
   * @return SelectQuery
   */
  protected function buildQuery($ids, $conditions = array(), $revision_id = FALSE) {
    $query = parent::buildQuery($ids, $conditions = array(), $revision_id = FALSE);

    $query->leftJoin('redhen_contact_user', 'rcu',
      'rcu.contact_id = base.contact_id AND rcu.redhen_state = :state',
      array(':state' => REDHEN_STATE_ACTIVE)
    );
    $query->fields('rcu', array('user_uid'));

    return $query;
  }

  /**
   * Set the active user for a contact.
   *
   * @param $contact
   */
  protected function setUser($contact) {
    $contact_user = array();
    $result = db_select('redhen_contact_user', 'cu')
      ->fields('cu')
      ->condition('contact_id', $contact->contact_id, '=')
      ->execute();
    if ($result) {
      $contact_user = reset($result->fetchAllAssoc('user_uid'));
    }

    // no user connection, create
    if (!$contact_user) {
      db_insert('redhen_contact_user')
        ->fields(array(
          'contact_id' => $contact->contact_id,
          'user_uid' => $contact->user_uid,
          'created' => REQUEST_TIME,
          'updated' => REQUEST_TIME
          ))
        ->execute();
    }
    // same user, bail. doesn't matter if connection is active or inactive
    else if ($contact_user->user_uid != $contact->user_uid) {
      // update with new user
      db_update('redhen_contact_user')
        ->fields(array(
            'user_uid' => $contact->user_uid,
            'redhen_state' => REDHEN_STATE_ACTIVE,
            'updated' => REQUEST_TIME
          ))
        ->condition('contact_id', $contact->contact_id)
        ->execute();
    }
  }

}
