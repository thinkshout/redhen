<?php

namespace Drupal\redhen_connection\Plugin\Action;

/**
 * Change the role of a Redhen Connection.
 *
 * @Action(
 *   id = "connection_change_role_action",
 *   label = @Translation("Change the role for the selected connection(s)"),
 *   type = "redhen_connection"
 * )
 */
class ChangeConnectionRole extends ChangeConnectionRoleBase {

  /**
   * {@inheritdoc}
   */
  public function execute($connection = NULL) {
    $role = $this->configuration['role'];
    // Skip changing the role to the user if they already have it.
    if ($connection !== FALSE && $connection->get('role', $role)->getString() != $role) {
      // For efficiency manually save the original account before applying
      // any changes.
      $connection->original = clone $connection;
      $connection->set('role', $role);
      $connection->save();
    }
  }

}
