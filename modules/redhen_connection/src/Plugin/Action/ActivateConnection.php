<?php

namespace Drupal\redhen_connection\Plugin\Action;

use Drupal\Core\Action\ActionBase;
use Drupal\Core\Session\AccountInterface;

/**
 * Activates a Connection.
 *
 * @Action(
 *   id = "connection_activate_connection_action",
 *   label = @Translation("Activate the selected connection(s)"),
 *   type = "redhen_connection"
 * )
 */
class ActivateConnection extends ActionBase {

  /**
   * {@inheritdoc}
   */
  public function execute($connection = NULL) {
    // Skip activating the connection already active.
    if ($connection !== FALSE && !$connection->isActive()) {
      $connection->setActive(TRUE);
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
