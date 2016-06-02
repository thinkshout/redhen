<?php

namespace Drupal\redhen_connection\Plugin\EntityReferenceSelection;

use Drupal\Core\Entity\Plugin\EntityReferenceSelection\DefaultSelection;

/**
 * Provides specific access control for the profile entity type.
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

    $connection_type = $this->configuration['entity']->getType();
    $query->condition('connection_type', $connection_type, '=');
    // Add connection_type parameter to the query.
    return $query;
  }


}