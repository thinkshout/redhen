<?php

namespace Drupal\redhen_connection\Plugin\Action;

use Drupal\Core\Action\ActionBase;
use Drupal\Core\Session\AccountInterface;

/**
 * Deactivates a Connection.
 *
 * @Action(
 *   id = "connection_deactivate_connection_action",
 *   label = @Translation("Deactivate the selected connection(s)"),
 *   type = "redhen_connection"
 * )
 */
class DeactivateConnection extends ActionBase {

  /**
   * {@inheritdoc}
   */
  public function execute($connection = NULL) {
    // Skip deactivating the connection if already inactive.
    if ($connection !== FALSE && $connection->isActive()) {
      // For efficiency manually save the original account before applying any
      // changes.
      $connection->original = clone $connection;
      $connection->setActive(FALSE);
      $connection->save();
    }
  }

  /**
   * {@inheritdoc}
   */
  public function access($object, AccountInterface $account = NULL, $return_as_object = FALSE) {
    /** @var \Drupal\redhen_connection\ConnectionInterface $object */
    $access = $object->status->access('edit', $account, TRUE);
    return $return_as_object ? $access : $access->isAllowed();
  }

}
