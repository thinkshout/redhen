<?php

/**
 * @file
 * The controller for the org type entity containing the CRUD operations.
 */

/**
 * The controller class for contact types contains methods for CRUD
 * operations. The load method is inherited from the default controller.
 */
class RedhenOrgTypeController extends EntityAPIControllerExportable {

  /**
   * Delete one or more org types.
   *
   * @param array $ids
   *   Array of org type IDs.
   * @param DatabaseTransaction $transaction
   *   Optionally a DatabaseTransaction object to use.
   */
  public function delete($ids, DatabaseTransaction $transaction = NULL) {
    $transaction = isset($transaction) ? $transaction : db_transaction();
    try {
      $query = new EntityFieldQuery();
      $result = $query
        ->entityCondition('entity_type', 'redhen_org')
        ->entityCondition('bundle', $ids, 'IN')
        ->execute();

      if ($result) {
        redhen_org_delete_multiple(array_keys($result['redhen_org']));
      }

      parent::delete($ids, $transaction);
    }
    catch(Exception $e) {
      $transaction->rollback();
      watchdog_exception($this->entityType, $e);
      throw $e;
    }
  }

}
