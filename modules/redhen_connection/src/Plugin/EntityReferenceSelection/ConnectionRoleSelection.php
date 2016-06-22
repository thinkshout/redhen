<?php

namespace Drupal\redhen_connection\Plugin\EntityReferenceSelection;

use Drupal\Core\Entity\Plugin\EntityReferenceSelection\DefaultSelection;

/**
 * Limit connection roles to those associated with this connection_type.
 *
 * @EntityReferenceSelection(
 *   id = "default:redhen_connection_role",
 *   label = @Translation("Connection role selection"),
 *   entity_types = {"redhen_connection_role"},
 *   group = "default",
 *   weight = 1
 * )
 */
class ConnectionRoleSelection extends DefaultSelection {

  /**
   * {@inheritdoc}
   */
  protected function buildEntityQuery($match = NULL, $match_operator = 'CONTAINS') {
    $query = parent::buildEntityQuery($match, $match_operator);

    // If the handler specifies a connection type...
    if (isset($this->configuration['handler_settings']['connection_type'])) {
      $connection_type = $this->configuration['handler_settings']['connection_type'];
      // Add connection_type parameter to the query.
      $query->condition('connection_type', $connection_type, '=');
    }
    return $query;
  }
}
