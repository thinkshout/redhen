<?php

/**
 * @file
 * The controller for the relation role entity containing the CRUD operations.
 */

/**
 * The controller class for contact types contains methods for CRUD
 * operations. The load method is inherited from the default controller.
 */
class RedHenRelationRoleEntityController extends EntityAPIControllerExportable {
  /**
   * Saves an relation role.
   *
   * @param RedHenRelationRole $relation_role
   *   The full relation role object to save.
   *
   * @return RedHenRelationRole
   *   The saved relation role object.
   */
  public function save($relation_role) {
    // If set to default, ensure only one role is default.
    $default_role = $relation_role->default_role;
    if ($default_role == 1) {
      $relation_role_id = isset($relation_role->redhen_relation_role_id) ?
        $relation_role->redhen_relation_role_id :
        0;
      $update_default = db_update('redhen_relation_role')
        ->fields(array(
          'default_role' => 0,
        ));
      if (isset($relation_role->is_new) && $relation_role->is_new) {
        $update_default
          ->condition('redhen_relation_role_id', $relation_role_id, '<>');
      }

      $update_default->execute();
    }

    parent::save($relation_role);

    return $relation_role;
  }
}