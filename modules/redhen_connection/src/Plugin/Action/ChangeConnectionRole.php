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
    $mismatch = FALSE;
    // Skip changing the role for the connection if already present,
    // OR if the role is not available for the given connection.
    $entity_storage = \Drupal::service('entity_type.manager')->getStorage('redhen_connection_role');
    $roles = array_keys($entity_storage->loadByProperties(['connection_type' => $connection->getType()]));
    if ($connection !== FALSE && $connection->get('role', $role)->getString() != $role) {
      if (in_array($role, $roles)) {
        $connection->original = clone $connection;
        $connection->set('role', $role);
        $connection->save();
      }
      else {
        $mismatch = TRUE;
      }
    }

    // If there are mismatched roles/connection_types provide a warning.
    if ($mismatch) {
      \Drupal::messenger()->addWarning($this->t('Some connections could not be updated because the selected role was not associated with the connection type.'));
    }
  }

}
