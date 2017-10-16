<?php

namespace Drupal\redhen_connection;

use Drupal\Core\Config\Entity\ConfigEntityInterface;

/**
 * Provides an interface for defining Connection type entities.
 */
interface ConnectionTypeInterface extends ConfigEntityInterface {
  /**
   * Gets the connection type's entity type ID by endpoint.
   *
   * E.g, if connections of this type are configured to allow Contacts to
   * connect with other Contacts, this will return redhen_contact for both
   * endpoints.
   *
   * @param int $num
   *   The endpoint to return the entity type for.
   *
   * @return string
   *   The entity type ID for endpoint $num.
   */
  public function getEndpointEntityTypeId($num);

  /**
   * Gets the connection type's endpoint label.
   *
   * @param int $num
   *   The endpoint to return the label for.
   *
   * @return string
   *   The label for endpoint $num.
   */
  public function getEndpointLabel($num);

  /**
   * Gets the connection type's endpoint description.
   *
   * @param int $num
   *   The endpoint to return the description for.
   *
   * @return string
   *   The description for endpoint $num.
   */
  public function getEndpointDescription($num);

  /**
   * Get all endpoint fields configured for $entity_type.
   *
   * @param string $entity_type
   * @param string $bundle
   *   (optional) Optional bundle to filter endpoints by.
   *
   * @return array
   *   The field names that reference $entity_type.
   */
  public function getEndpointFields($entity_type, $bundle = NULL);

  /**
   * Get all endpoint fields for this connection type.
   *
   * @return array
   *   The field names that reference connections.
   */
  public function getAllEndpointFields();

  /**
   * Gets the connection type's endpoint bundles.
   *
   * @param int $num
   *   The endpoint to return the description for.
   *
   * @return array()
   *   The array of enabled bundles.
   */
  public function getEndpointBundles($num);
}
