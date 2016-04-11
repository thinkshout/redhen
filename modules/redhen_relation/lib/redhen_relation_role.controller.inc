<?php

/**
 * @file
 * The controller for the relation role entity containing the CRUD operations.
 */

/**
 * The controller class for contact types contains methods for CRUD
 * operations. The load method is inherited from the default controller.
 */
class RedhenRelationRoleEntityController extends EntityAPIControllerExportable {
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
    parent::save($relation_role);

    return $relation_role;
  }
}
