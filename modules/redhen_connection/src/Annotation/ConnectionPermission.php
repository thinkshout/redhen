<?php

namespace Drupal\redhen_connection\Annotation;

use Drupal\Component\Annotation\Plugin;

/**
 * Defines a Connection permission item annotation object.
 *
 * @see \Drupal\redhen_connection\Plugin\ConnectionPermissionManager
 * @see plugin_api
 *
 * @Annotation
 */
class ConnectionPermission extends Plugin {


  /**
   * The plugin ID.
   *
   * @var string
   */
  public $id;

  /**
   * The label of the plugin.
   *
   * @var \Drupal\Core\Annotation\Translation
   *
   * @ingroup plugin_translatable
   */
  public $label;

  /**
   * The description of the plugin.
   *
   * @var \Drupal\Core\Annotation\Translation
   *
   * @ingroup plugin_translatable
   */
  public $description;

  /**
   * The subject entity type, or the entity for which we are managing access.
   *
   * @var string
   */
  public $subject_entity_type;

  /**
   * The subject entity bundle (optional).
   *
   * @var string
   */
  public $subject_entity_bundle;

  /**
   * The influencer entity type, or the entity type that will influence access.
   *
   * @var string
   */
  public $influencer_entity_type;

}
