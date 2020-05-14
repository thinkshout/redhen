<?php

namespace Drupal\redhen_connection\Entity;

use Drupal\Core\Config\Entity\ConfigEntityBundleBase;
use Drupal\redhen_connection\ConnectionTypeInterface;

/**
 * Defines the Connection type entity.
 *
 * @ConfigEntityType(
 *   id = "redhen_connection_type",
 *   label = @Translation("Connection type"),
 *   handlers = {
 *     "list_builder" = "Drupal\redhen_connection\ConnectionTypeListBuilder",
 *     "form" = {
 *       "add" = "Drupal\redhen_connection\Form\ConnectionTypeForm",
 *       "edit" = "Drupal\redhen_connection\Form\ConnectionTypeForm",
 *       "delete" = "Drupal\redhen_connection\Form\ConnectionTypeDeleteForm"
 *     },
 *     "route_provider" = {
 *       "html" = "Drupal\redhen_connection\ConnectionTypeHtmlRouteProvider",
 *     },
 *   },
 *   config_prefix = "redhen_connection_type",
 *   admin_permission = "administer site configuration",
 *   bundle_of = "redhen_connection",
 *   entity_keys = {
 *     "id" = "id",
 *     "label" = "label",
 *     "uuid" = "uuid"
 *   },
 *   links = {
 *     "canonical" = "/admin/structure/redhen/connection_type/{redhen_connection_type}",
 *     "add-form" = "/admin/structure/redhen/connection_type/add",
 *     "edit-form" = "/admin/structure/redhen/connection_type/{redhen_connection_type}/edit",
 *     "delete-form" = "/admin/structure/redhen/connection_type/{redhen_connection_type}/delete",
 *     "collection" = "/admin/structure/redhen/connection_type"
 *   }
 * )
 */
class ConnectionType extends ConfigEntityBundleBase implements ConnectionTypeInterface {
  /**
   * The Connection type ID.
   *
   * @var string
   */
  protected $id;

  /**
   * The Connection type label.
   *
   * @var string
   */
  protected $label;

  /**
   * The Connection label pattern.
   *
   * @var string
   */
  protected $connection_label_pattern;

  /**
   * The endpoint definitions.
   *
   * @var array
   */
  protected $endpoints = [];

  /**
   * {@inheritdoc}
   */
  public function getEndpointEntityTypeId($num) {
    $entity_type = NULL;
    if (isset($this->endpoints[$num]['entity_type'])) {
      $entity_type = $this->endpoints[$num]['entity_type'];
    }

    return $entity_type;
  }

  /**
   * {@inheritdoc}
   */
  public function getEndpointLabel($num) {
    $label = NULL;
    if (isset($this->endpoints[$num]['label'])) {
      $label = $this->endpoints[$num]['label'];
    }

    return $label;
  }

  /**
   * {@inheritdoc}
   */
  public function getEndpointDescription($num) {
    $description = NULL;
    if (isset($this->endpoints[$num]['description'])) {
      $description = $this->endpoints[$num]['description'];
    }

    return $description;
  }

  /**
   * {@inheritdoc}
   */
  public function getEndpointFields($entity_type, $bundle = NULL) {
    $fields = [];
    foreach ($this->endpoints as $id => $endpoint) {
      if (($endpoint['entity_type'] === $entity_type) &&
        (!$bundle || in_array($bundle, $endpoint['bundles']))) {
        $fields[] = 'endpoint_' . $id;
      }
    }

    return $fields;
  }

  /**
   * {@inheritdoc}
   */
  public function getAllEndpointFields() {
    $fields = [];
    foreach ($this->endpoints as $id => $endpoint) {
      $fields[] = 'endpoint_' . $id;
    }

    return $fields;
  }

  /**
   * {@inheritdoc}
   */
  public function getEndpointBundles($num) {
    $bundles = NULL;
    if (isset($this->endpoints[$num]['bundles'])) {
      $bundles = $this->endpoints[$num]['bundles'];
    }

    return $bundles;
  }

  /**
   * {@inheritdoc}
   */
  public function getEndpointEntityTypes($entity_type_id, $bundle) {
    // If the current entity type is in a connection endpoint.
    if ($this->getEndpointEntityTypeId(1) === $entity_type_id || $this->getEndpointEntityTypeId(2) === $entity_type_id) {
      // If the current entity bundle is in a connection endpoint bundle.
      $endBundles1 = $this->getEndpointBundles(1);
      $bundle1 = reset($endBundles1);
      $endBundles2 = $this->getEndpointBundles(2);
      $bundle2 = reset($endBundles2);
      if ($bundle1 === $bundle || $bundle2 === $bundle) {
        return $entity_type_id;
      }
    }
    return FALSE;
  }

}
